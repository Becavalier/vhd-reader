#include <iostream>
#include <fstream>
#include <sstream> 
#include <string>

constexpr int POS_HEAD_FILE = 1;
constexpr int BUF_SIZE_DISK_GEO = 4;

struct DiskGeometry {
  /**
   * Cylinder - 2 bytes.
   * Heads - 1 bytes.
   * Sectors per track/cylinder	- 1 byte.
  */
  uint16_t cylinder;
  unsigned int heads;
  unsigned int sectorPerTrack;
  DiskGeometry(std::ifstream &in) {
    char buf[BUF_SIZE_DISK_GEO];
    in.seekg(static_cast<int>(in.tellg()) + 48);
    in.read(buf, sizeof(buf));
    cylinder = static_cast<unsigned char>(buf[0]) << 8 | static_cast<unsigned char>(buf[1]);  // Compose data in Big Endian.
    heads = buf[2];
    sectorPerTrack = buf[3];
  }
};

int main(int argc, char* argv[]) {
  if (argc < 2) return 1;
  const std::string fileName(argv[POS_HEAD_FILE]);
  std::ifstream in { fileName, std::ifstream::binary };
  if (in) {
    // Re-position to the footer section.
    in.seekg(0, in.end);
    const int fileLen = in.tellg();
    const int basePos = fileLen - 512;
    in.seekg(basePos);

    // Verify cookie.
    char buffer[8];
    in.read(buffer, sizeof(buffer));

    if (std::string(buffer) == "conectix") {
      // Read the "Disk Geometry" field (4 bytes in total).
      DiskGeometry diskGeometry(in);
      std::stringstream ss;
      ss << "[Disk Geometry]\n"
        << "\nCylinder: " << diskGeometry.cylinder \
        << "\nHeads: " << diskGeometry.heads \
        << "\nSector length (bytes): 512" \
        << "\nSectors per track/cylinder: " << diskGeometry.sectorPerTrack;
      std::cout << ss.str() << std::endl;
    }
    in.close();
  }
  return 0;
}


