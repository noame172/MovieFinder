using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Client
{
    public class Response
    {
        public uint code { get; set; }
        public string title { get; set; }
        public float runTimeInMinutes { get; set; }
        public List<string> genres { get; set; }
        public string overview { get; set; }

        public string moviePosterPath { get; set; }
    }
}
