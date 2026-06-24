using Client;
using Newtonsoft.Json;
using System.Text;

namespace Client
{
    public class Deserializer
    {
        public static Response DeserializeResponse(uint code, byte[] data)
        {
            string json = Encoding.UTF8.GetString(data);

            var jsonObject = Newtonsoft.Json.Linq.JObject.Parse(json);

            string movieJson = jsonObject["movie"]?.ToString() ?? json;

            Response response = JsonConvert.DeserializeObject<Response>(movieJson);

            if (response == null)
            {
                response = new Response();
            }

            response.code = code;

            return response;
        }
    }
}