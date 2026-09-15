#include "lib/NavierStokesEquations/io.h"

#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "lib/NavierStokesEquations/diagnostics.h"
#include "lib/NavierStokesEquations/grid.h"

#include "gtest/gtest.h"

using namespace navierStokes;

namespace {

// Writes go to a scratch file that is removed again when the test ends, so the
// suite leaves nothing behind whether it passes or fails.
class ScratchFile {
   public:
    explicit ScratchFile(const std::string& name) : path_(testing::TempDir() + "nsIoTest_" + name) {}
    ~ScratchFile() { std::remove(path_.c_str()); }

    const std::string& path() const { return path_; }

    std::string contents() const {
        std::ifstream in(path_, std::ios::binary);
        std::ostringstream buffer;
        buffer << in.rdbuf();
        return buffer.str();
    }

    std::vector<std::string> lines() const {
        std::vector<std::string> result;
        std::istringstream in(contents());
        std::string line;
        while (std::getline(in, line)) {
            result.push_back(line);
        }
        return result;
    }

   private:
    std::string path_;
};

std::vector<double> parseRow(const std::string& line) {
    std::vector<double> values;
    std::istringstream in(line);
    std::string cell;
    while (std::getline(in, cell, ',')) {
        values.push_back(cell.empty() ? 0.0 : std::stod(cell));
    }
    return values;
}

}  // namespace

// One line per row of constant y, y ascending, so numpy.loadtxt gives an array
// indexed as [j][i] that imshow(origin="lower") shows the right way up.
TEST(Io, FieldCsvHasOneLinePerRowInAscendingY) {
    const StaggeredGrid grid(3, 2, 1.0, 1.0);
    Field2D field(grid.nx, grid.ny, 0);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            field(i, j) = 10.0 * j + i;
        }
    }

    const ScratchFile file("field.csv");
    ASSERT_TRUE(writeFieldCsv(file.path(), field));

    const std::vector<std::string> lines = file.lines();
    ASSERT_EQ(lines.size(), 2u);
    EXPECT_EQ(lines[0], "0,1,2");
    EXPECT_EQ(lines[1], "10,11,12");
}

TEST(Io, VelocityCsvHasAHeaderAndOneRowPerCell) {
    const StaggeredGrid grid(4, 3, 2.0, 1.0);
    Field2D u(grid.nx + 1, grid.ny, 1);
    Field2D v(grid.nx, grid.ny + 1, 1);
    u.fill(3.0);
    v.fill(4.0);

    const ScratchFile file("velocity.csv");
    ASSERT_TRUE(writeVelocityCsv(file.path(), grid, u, v));

    const std::vector<std::string> lines = file.lines();
    ASSERT_EQ(lines.size(), 1u + static_cast<size_t>(grid.nx * grid.ny));
    EXPECT_EQ(lines[0], "x,y,u,v,speed");

    const std::vector<double> first = parseRow(lines[1]);
    ASSERT_EQ(first.size(), 5u);
    EXPECT_NEAR(first[0], grid.xP(0), 1e-9);
    EXPECT_NEAR(first[1], grid.yP(0), 1e-9);
    EXPECT_NEAR(first[2], 3.0, 1e-9);
    EXPECT_NEAR(first[3], 4.0, 1e-9);
    EXPECT_NEAR(first[4], 5.0, 1e-9);  // hypot(3, 4)
}

TEST(Io, ColumnsCsvPadsShortColumns) {
    const ScratchFile file("columns.csv");
    const std::vector<std::string> headers = {"n", "error", "order"};
    const std::vector<std::vector<double>> columns = {{16, 32, 64}, {1e-2, 2.5e-3, 6.25e-4}, {2.0, 2.0}};
    ASSERT_TRUE(writeColumnsCsv(file.path(), headers, columns));

    const std::vector<std::string> lines = file.lines();
    ASSERT_EQ(lines.size(), 4u);
    EXPECT_EQ(lines[0], "n,error,order");
    EXPECT_EQ(parseRow(lines[1]).size(), 3u);
    // The order column runs out one row early and its cell is left empty.
    EXPECT_EQ(lines[3].back(), ',');
    const std::vector<double> lastRow = parseRow(lines[3]);
    EXPECT_NEAR(lastRow[0], 64.0, 1e-9);
    EXPECT_NEAR(lastRow[1], 6.25e-4, 1e-12);
}

TEST(Io, PpmHasABinaryHeaderAndThreeBytesPerPixel) {
    const StaggeredGrid grid(5, 4, 1.0, 1.0);
    Field2D field(grid.nx, grid.ny, 0);
    for (int i = 0; i < grid.nx; ++i) {
        for (int j = 0; j < grid.ny; ++j) {
            field(i, j) = i - 2.0;
        }
    }

    const ScratchFile file("image.ppm");
    ASSERT_TRUE(writePpm(file.path(), field, -2.0, 2.0));

    const std::string contents = file.contents();
    const std::string header = "P6\n5 4\n255\n";
    ASSERT_GE(contents.size(), header.size());
    EXPECT_EQ(contents.substr(0, header.size()), header);
    EXPECT_EQ(contents.size(), header.size() + 3u * 5u * 4u);

    // The middle column sits at the centre of the range and comes out white.
    const unsigned char* pixels = reinterpret_cast<const unsigned char*>(contents.data() + header.size());
    const size_t middle = 3u * 2u;  // first row of the image, third pixel
    EXPECT_EQ(pixels[middle + 0], 255);
    EXPECT_EQ(pixels[middle + 1], 255);
    EXPECT_EQ(pixels[middle + 2], 255);
    // The ends of the range are the extremes of the diverging map: the low end
    // is blue-dominant and the high end is red-dominant.
    EXPECT_LT(pixels[0], pixels[2]);
    const size_t last = 3u * 4u;
    EXPECT_GT(pixels[last + 0], pixels[last + 2]);
}

TEST(Io, PpmPaintsMaskedCellsGrey) {
    const StaggeredGrid grid(4, 4, 1.0, 1.0);
    Field2D field(grid.nx, grid.ny, 0);
    field.fill(1.0);
    Field2D mask(grid.nx, grid.ny, 0);
    mask(1, 3) = 1.0;  // top row of the image, second pixel

    const ScratchFile file("masked.ppm");
    ASSERT_TRUE(writePpm(file.path(), field, -1.0, 1.0, &mask));

    const std::string contents = file.contents();
    const std::string header = "P6\n4 4\n255\n";
    const unsigned char* pixels = reinterpret_cast<const unsigned char*>(contents.data() + header.size());
    const size_t masked = 3u * 1u;  // rows are flipped, so j = ny - 1 comes first
    EXPECT_EQ(pixels[masked + 0], 110);
    EXPECT_EQ(pixels[masked + 1], 110);
    EXPECT_EQ(pixels[masked + 2], 110);
    EXPECT_NE(pixels[0], 110);
}

TEST(Io, WritersReportAnUnwritablePath) {
    const std::string bad = "/nsIoTestNoSuchDirectory/out";
    Field2D field(2, 2, 0);
    EXPECT_FALSE(writeFieldCsv(bad, field));
    EXPECT_FALSE(writePpm(bad, field, 0.0, 1.0));
    EXPECT_FALSE(writeColumnsCsv(bad, {"a"}, {{1.0}}));
    const StaggeredGrid grid(2, 2, 1.0, 1.0);
    Field2D u(3, 2, 1);
    Field2D v(2, 3, 1);
    EXPECT_FALSE(writeVelocityCsv(bad, grid, u, v));
}
