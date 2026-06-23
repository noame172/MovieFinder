import socket
import json
import struct

def serialize_get_movie_request(movie_name):
    request_data = {"movieName": movie_name}
    json_payload = json.dumps(request_data).encode('utf-8')
    payload_size = len(json_payload)
    return b"1" + f"{payload_size:04d}".encode('utf-8') + json_payload

def deserialize_get_movie_response(header_bytes, payload_bytes):
    res_code, res_status, res_size = struct.unpack('!BBI', header_bytes)
    
    # Combine all received data
    full_payload = header_bytes[6:] + payload_bytes
    
    # Safeguard: slice by actual size reported to prevent crash
    json_bytes = full_payload[:res_size]
    
    try:
        return res_code, res_status, json.loads(json_bytes.decode('utf-8', errors='ignore'))
    except Exception as je:
        # If JSON is corrupted (e.g. status 0), return raw data for debugging
        return res_code, res_status, {"raw_error_data": json_bytes.decode('utf-8', errors='ignore')[:500]}

def send_get_movie_request(server_ip, port, movie_name):
    print(f"[CLIENT] Connecting to server {server_ip}:{port}...")
    
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            # Setting a 2-second timeout so it never hangs indefinitely
            client_socket.settimeout(2.0)
            client_socket.connect((server_ip, port))
            
            # 1. Send the request
            serialized_packet = serialize_get_movie_request(movie_name)
            client_socket.sendall(serialized_packet)
            
            # 2. Read the Header
            print("[CLIENT] Waiting for server response...")
            header_bytes = client_socket.recv(6)
            if not header_bytes or len(header_bytes) < 6:
                print("[CLIENT] Server closed connection or sent invalid header.")
                return
                
            res_code, res_status, res_size = struct.unpack('!BBI', header_bytes)
            print(f"[DEBUG] Header parsed -> Code: {res_code}, Status: {res_status}, Expected JSON Size: {res_size}")
            
            # Sanity check: if size is unrealistically large (>5MB), cap it
            if res_size > 5000000:
                print("[WARNING] Server reported an unrealistically large size. Capping read.")
                res_size = 65535
            
            # 3. Read the rest of the JSON payload safely
            bytes_already_received = max(0, len(header_bytes) - 6)
            bytes_to_read = res_size - bytes_already_received
            
            payload_bytes = b""
            try:
                while len(payload_bytes) < bytes_to_read:
                    # Read in chunks of max 4KB at a time
                    chunk_size = min(4096, bytes_to_read - len(payload_bytes))
                    packet = client_socket.recv(chunk_size)
                    if not packet:
                        break
                    payload_bytes += packet
            except socket.timeout:
                print("[TIMEOUT] Reached timeout while reading full payload. Parsing partial data...")

            # 4. Deserialize and display specific fields
            res_code, res_status, response_json = deserialize_get_movie_response(header_bytes, payload_bytes)

            print(f"\n[CLIENT] Deserialization Complete!")
            print(f"-> Response Status: {res_status}")

            # Check if the response status is successful and the movie key exists
            if res_status == 1 and "movie" in response_json:
                movie_info = response_json["movie"]
                
                # Safely extract specific fields using .get()
                title = movie_info.get("title", "Unknown")
                runtime = movie_info.get("runtime", 0)
                overview = movie_info.get("overview", "No overview available.")
                genres = movie_info.get("genres", [])
                
                # Format and print selected data cleanly
                print("\n--- MOVIE DETAILS FOUND ---")
                print(f"🎬 Title: {title}")
                print(f"⏱️ Runtime: {runtime} minutes")
                print(f"🎭 Genres: {', '.join(genres)}")
                print(f"📝 Overview: {overview}")
                print("---------------------------\n")
                
            else:
                # Handle cases where the server returned an error or partial structure
                if "raw_error_data" in response_json:
                    print("[INFO] Received raw error data from server (potential C++ parsing failure):")
                    print(response_json["raw_error_data"])
                else:
                    print("[ERROR] Server returned an empty response, or the movie was not found.")
            
            # Gracefully signal the server we are disconnecting
            try:
                client_socket.sendall(b"0")
            except:
                pass

    except socket.timeout:
        print("[ERROR] Server did not respond within the timeout limit.")
    except Exception as e:
        print(f"\n[ERROR] Something went wrong during packet processing: {e}")

if __name__ == "__main__":
    send_get_movie_request("127.0.0.1", 5000, "The Matrix")