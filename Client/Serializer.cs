using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Nodes;
using System.Threading.Tasks;
using System.Text.Json;

namespace Client
{
    internal class Serializer
    {
        public static byte[] SerializeRequest(byte code, string data)
        {
            byte[] dataBytes = Encoding.UTF8.GetBytes(data);
            int size = dataBytes.Length;

            byte[] buffer = new byte[1 + 4 + size];
            buffer[0] = code;

            buffer[1] = (byte)(size >> 24);
            buffer[2] = (byte)(size >> 16);
            buffer[3] = (byte)(size >> 8);
            buffer[4] = (byte)(size);


            Array.Copy(dataBytes, 0, buffer, 5, size);

            return buffer;
        }
    }
}
