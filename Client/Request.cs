using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client
{
    internal class Request
    {
        private int _code;
        private string _data;
        public Request(int code, string data)
        {
            _code = code;
            _data = data;
        }

    }
}
