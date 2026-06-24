using Newtonsoft.Json;
using System;
using System.Net.Sockets;
using System.Text;

namespace Client
{
    public class Communicator
    {
        private TcpClient m_socket;
        private NetworkStream m_stream;

        public Communicator()
        {
            m_socket = new TcpClient();
        }

        public void connect(string ip, int port)
        {
            m_socket.Connect(ip, port);
            m_stream = m_socket.GetStream();

            m_socket.ReceiveTimeout = 5000;
            m_socket.SendTimeout = 5000;
        }

        private void sendRequest(byte code, string data)
        {
            byte[] buffer = Serializer.SerializeRequest(code, data);
            m_stream.Write(buffer, 0, buffer.Length);
            m_stream.Flush();
        }

        private void readExact(byte[] buffer, int count)
        {
            int totalRead = 0;
            while (totalRead < count)
            {
                int bytesRead = m_stream.Read(buffer, totalRead, count - totalRead);
                if (bytesRead == 0)
                    throw new Exception("Connection closed by server.");
                totalRead += bytesRead;
            }
        }

        private Response readResponse()
        {
            byte[] codeBuffer = new byte[1];
            readExact(codeBuffer, 1);
            uint code = codeBuffer[0];

            byte[] sizeBuffer = new byte[4];
            readExact(sizeBuffer, 4);

            int size = (sizeBuffer[0] << 24) | (sizeBuffer[1] << 16) |
                       (sizeBuffer[2] << 8) | sizeBuffer[3];

            byte[] dataBuffer = new byte[size];
            readExact(dataBuffer, size);

            return Deserializer.DeserializeResponse(code, dataBuffer);
        }

        public Response sendAndReceive<T>(RequestCode code, T data)
        {
            try
            {
                string json = data != null ? JsonConvert.SerializeObject(data) : "{}";
                sendRequest((byte)code, json);
                return readResponse();
            }
            catch (Exception e)
            {
                throw new Exception("Error in communicator: " + e.Message);
            }
        }

        public void disconnect()
        {
            m_stream?.Close();
            m_stream = null;
            m_socket?.Close();
            m_socket = null;
        }
    }
}