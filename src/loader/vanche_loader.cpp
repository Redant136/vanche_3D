#include "vanche_loader.hpp"
#include <chevan_utils_macro.h>
#include <nlohmann/json.hpp>
#include <chevan_utils_print.hpp>

typedef nlohmann::json json;
struct BIN_t
{
  uchar *data;
  size_t size;
};

static uchar *pop_buff(uchar **data, size_t length)
{
  uchar *a = *data;
  *data += length;
  return a;
}
#define pop_buff(data, length) pop_buff((uchar **)&(data), (length))

static ch_hash deserialize_extensions(json extensions);
static ch_hash deserialize_extras(json extensions);
static ch_hash deserialize_extensions(json extensions){

}
static ch_hash deserialize_extras(json extensions){

}

static gltf::glTFModel parseGLTFJSON(json data)
{
#define jsonget(json,dst,name) if(json.contains(#name)){dst.name=json[#name].get<typeof(dst.name)>();}
  gltf::glTFModel gltf;
  if(data.contains("asset")){
    jsonget(data["asset"], gltf.asset, version);
    jsonget(data["asset"], gltf.asset,generator);
    jsonget(data["asset"],gltf.asset,copyright);
    jsonget(data["asset"], gltf.asset,minVersion);
    gltf.asset.extensions = deserialize_extensions(data["asset"]);
    gltf.asset.extras = deserialize_extras(data["asset"]);
  }
  if(data.contains("buffers")){
    for(int i=0;i<data["buffers"].size();i++){
      gltf::Buffer b;
      jsonget(data["buffers"][i], b, uri);
      jsonget(data["buffers"][i], b, byteLength);
      jsonget(data["buffers"][i], b, name);
      b.extensions=deserialize_extensions(data["buffers"][i]);
      b.extras=deserialize_extras(data["buffers"][i]);
      gltf.buffers.push_back(b);
    }
  }
  if(data.contains("bufferViews")){
    for(int i=0;i<data["bufferViews"].size();i++){
      gltf::BufferView bv;
      jsonget(data["bufferViews"][i],bv,buffer);
      jsonget(data["bufferViews"][i],bv,byteOffset);
      jsonget(data["bufferViews"][i],bv,byteLength);
      jsonget(data["bufferViews"][i],bv,byteStride);
      jsonget(data["bufferViews"][i],bv,target);
      jsonget(data["bufferViews"][i],bv,name);
      bv.extensions=deserialize_extensions(data["bufferViews"][i]);
      bv.extras = deserialize_extras(data["bufferViews"][i]);
      gltf.bufferViews.push_back(bv);
    }
  }
  if (data.contains("accessors")){
    for (int i = 0; i < data["accessors"].size();i++){
      gltf::Accessor ac;
      jsonget(data["accessors"][i],ac, bufferView);
      jsonget(data["accessors"][i], ac, byteOffset);
      if(data["accessors"][i].contains("type")){
        ac.type = CH_ENUM_PARSE(data["accessors"][i]["type"].get<std::string>().c_str(), gltf::Accessor::Types::, SCALAR, VEC2,VEC3,VEC4,MAT2,MAT3,MAT4);
      }
      jsonget(data["accessors"][i], ac, componentType);
      jsonget(data["accessors"][i], ac, count);
      jsonget(data["accessors"][i], ac, max);
      jsonget(data["accessors"][i], ac, min);
      jsonget(data["accessors"][i], ac, normalized);
      if (data["accessors"][i].contains("sparse"))
      {
        jsonget(data["accessors"][i]["sparse"], ac.sparse, count);
        if (data["accessors"][i]["sparse"].contains("indices"))
        {
          jsonget(data["accessors"][i]["sparse"]["indices"],ac.sparse.indices,bufferView);
          jsonget(data["accessors"][i]["sparse"]["indices"], ac.sparse.indices, byteOffset);
          jsonget(data["accessors"][i]["sparse"]["indices"], ac.sparse.indices, componentType);
          ac.sparse.indices.extensions = deserialize_extensions(data["accessors"][i]["sparse"]["indices"]);
          ac.sparse.indices.extensions = deserialize_extras(data["accessors"][i]["sparse"]["indices"]);
        }
        if (data["accessors"][i]["sparse"].contains("values"))
        {
          jsonget(data["accessors"][i]["sparse"]["values"], ac.sparse.values, bufferView);
          jsonget(data["accessors"][i]["sparse"]["values"], ac.sparse.values, byteOffset);
          ac.sparse.values.extensions = deserialize_extensions(data["accessors"][i]["sparse"]["values"]);
          ac.sparse.values.extensions = deserialize_extras(data["accessors"][i]["sparse"]["values"]);
        }
        ac.sparse.extensions=deserialize_extensions(data["accessors"][i]["sparse"]);
        ac.sparse.extras = deserialize_extras(data["accessors"][i]["sparse"]);
      }
      jsonget(data["accessors"][i], ac, name);
      ac.extensions=deserialize_extensions(data["accessors"][i]);
      ac.extras = deserialize_extras(data["accessors"][i]);
      gltf.accessors.push_back(ac);
    }
  }
  if (data.contains("textures")){
    for(int i=0;i<data["textures"].size();i++){
      gltf::Texture tex;
      jsonget(data["textures"],tex,sampler);
      jsonget(data["textures"], tex, source);
      jsonget(data["textures"], tex, name);
      tex.extensions=deserialize_extensions(data["textures"]);
      tex.extras = deserialize_extras(data["textures"]);
      gltf.textures.push_back(tex);
    }
  }
  if (data.contains("samplers")){
    for(int i=0;i<data["samplers"].size();i++){
    gltf::Sampler s;
    jsonget(data["samplers"][i], s, magFilter);
        gltf.samplers.push_back(s);
    }
  }
#undef jsonget
  return gltf;
}

namespace base64
{
  static unsigned int pos_of_char(const unsigned char chr)
  {
    //
    // Return the position of chr within base64_encode()
    //

    if (chr >= 'A' && chr <= 'Z')
      return chr - 'A';
    else if (chr >= 'a' && chr <= 'z')
      return chr - 'a' + ('Z' - 'A') + 1;
    else if (chr >= '0' && chr <= '9')
      return chr - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    else if (chr == '+' || chr == '-')
      return 62; // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters (
    else if (chr == '/' || chr == '_')
      return 63; // Ditto for '/' and '_'
    else
      //
      // 2020-10-23: Throw std::exception rather than const char*
      //(Pablo Martin-Gomez, https://github.com/Bouska)
      //
      throw std::runtime_error("Input is not valid base64-encoded data.");
  }
  static std::string decode(std::string, bool);
  static std::string base64_decode(std::string const &s, bool remove_linebreaks = false)
  {
    return decode(s, remove_linebreaks);
  }
  static std::string decode(std::string encoded_string, bool remove_linebreaks = false)
  {
    //
    // decode(…) is templated so that it can be used with String = const std::string&
    // or std::string_view (requires at least C++17)
    //

    if (encoded_string.empty())
      return std::string();

    if (remove_linebreaks)
    {

      std::string copy(encoded_string);

      copy.erase(std::remove(copy.begin(), copy.end(), '\n'), copy.end());

      return base64_decode(copy, false);
    }

    size_t length_of_string = encoded_string.length();
    size_t pos = 0;

    //
    // The approximate length (bytes) of the decoded string might be one or
    // two bytes smaller, depending on the amount of trailing equal signs
    // in the encoded string. This approximation is needed to reserve
    // enough space in the string to be returned.
    //
    size_t approx_length_of_decoded_string = length_of_string / 4 * 3;
    std::string ret;
    ret.reserve(approx_length_of_decoded_string);

    while (pos < length_of_string)
    {
      //
      // Iterate over encoded input string in chunks. The size of all
      // chunks except the last one is 4 bytes.
      //
      // The last chunk might be padded with equal signs or dots
      // in order to make it 4 bytes in size as well, but this
      // is not required as per RFC 2045.
      //
      // All chunks except the last one produce three output bytes.
      //
      // The last chunk produces at least one and up to three bytes.
      //

      size_t pos_of_char_1 = pos_of_char(encoded_string[pos + 1]);

      //
      // Emit the first output byte that is produced in each chunk:
      //
      ret.push_back(static_cast<std::string::value_type>(((pos_of_char(encoded_string[pos + 0])) << 2) + ((pos_of_char_1 & 0x30) >> 4)));

      if ((pos + 2 < length_of_string) && // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
          encoded_string[pos + 2] != '=' &&
          encoded_string[pos + 2] != '.' // accept URL-safe base 64 strings, too, so check for '.' also.
      )
      {
        //
        // Emit a chunk's second byte (which might not be produced in the last chunk).
        //
        unsigned int pos_of_char_2 = pos_of_char(encoded_string[pos + 2]);
        ret.push_back(static_cast<std::string::value_type>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

        if ((pos + 3 < length_of_string) &&
            encoded_string[pos + 3] != '=' &&
            encoded_string[pos + 3] != '.')
        {
          //
          // Emit a chunk's third byte (which might not be produced in the last chunk).
          //
          ret.push_back(static_cast<std::string::value_type>(((pos_of_char_2 & 0x03) << 6) + pos_of_char(encoded_string[pos + 3])));
        }
      }

      pos += 4;
    }

    return ret;
  }
}

static gltf::glTFModel parseGLTF(std::string path, BIN_t bin)
{
  gltf::glTFModel model;

  auto json = json::parse(bin.data, bin.data + bin.size);
  model = parseGLTFJSON(json);

  std::string dir = "./";
  {
    char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif
    size_t sepIndex = path.rfind(sep, path.length());
    if (sepIndex != std::string::npos)
    {
      dir = path.substr(0, sepIndex);
    }
  }

  for (uint i = 0; i < model.buffers.size(); i++)
  {
    if (model.buffers[i].uri.find("data:") == 0) // data URI
    {
      if (model.buffers[i].uri.find("base64") == std::string::npos)
      {
        assert(0 && "can only decode base64");
      }
      std::string data = model.buffers[i].uri.substr(model.buffers[i].uri.find(",") + 1);
      data = base64::decode(data);
      model.buffers[i].buffer = new uchar[data.length()];
      memcpy(model.buffers[i].buffer, data.data(), data.length());
    }
    else // file based
    {
      std::string file = dir + "/" + model.buffers[i].uri;
      std::ifstream input(file, std::ios::binary);
      std::vector<char> bytes(
          (std::istreambuf_iterator<char>(input)),
          (std::istreambuf_iterator<char>()));
      input.close();

      model.buffers[i].buffer = new uchar[bytes.size()];
      memcpy(model.buffers[i].buffer, bytes.data(), bytes.size());
    }
  }

  for (uint i = 0; i < model.images.size(); i++)
  {
    // TODO(ANT) images ig
  }

  return model;
}
static gltf::glTFModel parseGLB(std::string path, BIN_t bin)
{
  gltf::glTFModel model;
  if (bin.size <= 12)
  {
    chprinterr("Error loading file at %s:%d.\nFile seems to be empty\n", __FILE__, __LINE__);
  }
  membuild(
      struct {
        char glTF[4];
        uint32_t version;
        uint32_t dataSize;
      },
      header, pop_buff(bin.data, 12));
  printf("Loading model:%s\nglTF version %d size: %d, magic: %s\n", path.c_str(), header.version, header.dataSize, std::string(header.glTF, 4).c_str());
  if (memcmp("glTF", header.glTF, 4))
  {
    chprinterr("Error loading file at %s:%d.\nUnexpected magic: %s. File might be corrupted\n", __FILE__, __LINE__, header.glTF);
  }
  bin.size -= sizeof(header);

  struct chunk_t
  {
    uint32_t length;
    uint32_t type;
    uchar *data;
  };
#define JSON_GLB_CHUNK 0x4E4F534A
#define BIN_GLB_CHUNK 0x004E4942
  int dataSize = 12, nChunks = 0;
  chunk_t chunks[2] = {{0}, {0}};
  while (dataSize < header.dataSize)
  {
    chunk_t c = {CHVAL, CHVAL, 0};
    memcpy(&c, pop_buff(bin.data, 8), 8);
    chassert(c.length != CHVAL && c.type != CHVAL,"Could not read data length or type from buffer")
    c.data = (uchar *)malloc(c.length);
    memcpy(c.data, pop_buff(bin.data, c.length), c.length);
    dataSize += 8 + c.length;
    if (c.type == JSON_GLB_CHUNK)
      chunks[0] = c;
    else if (c.type == BIN_GLB_CHUNK)
      chunks[1] = c;
    else
      chprinterr("Error during loading file chunk %d at %s:%d.\nUnexpected chunk type:%X. File might be corrupted\n", nChunks, __FILE__, __LINE__, c.type);
  }

  json jsonData = json::parse(chunks[0].data, chunks[0].data + chunks[0].length);
  model = parseGLTFJSON(jsonData);

  if (model.buffers.size() != 1)
  {
    chprinterr("Only 1 buffer should be present in file");
  }
  model.buffers[0].buffer = (uchar *)malloc(chunks[1].length);
  memcpy(model.buffers[0].buffer, chunks[1].data, chunks[1].length);
  free(chunks[0].data);
  free(chunks[1].data);
  return model;
}

gltf::glTFModel loadModel(std::string path)
{
  Filetype filetype;
  gltf::glTFModel model;
  const char *dot = strrchr(path.c_str(), '.') + 1;
  if (!strcmp("glb", dot))
  {
    filetype = Filetype::glb;
  }
  else if (!strcmp("vrm", dot))
  {
    filetype = Filetype::vrm;
  }
  else if (!strcmp("gltf", dot))
  {
    filetype = Filetype::gltf;
  }
  else
  {
    filetype = Filetype::unknown;
  }

  BIN_t bin = {0, 0};
  bin.data = (uchar *)ch_bufferFile(path.c_str(), (void **)&bin.data, &bin.size);
  if (!bin.data)
  {
    chprinterr("Could not open file:%s\n", path.c_str());
  }

  switch (filetype)
  {
  case Filetype::vrm:
  case Filetype::glb:
    model = parseGLB(path, bin);
    break;
  case Filetype::gltf:
    model = parseGLTF(path, bin);
    break;
  default:
    break;
  }
  free(bin.data);
  return model;
}
