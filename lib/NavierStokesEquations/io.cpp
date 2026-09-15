#include "lib/NavierStokesEquations/io.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fstream>
#include <ios>

namespace navierStokes {
namespace {

unsigned char toByte(double value) {
    const double scaled = std::round(255.0 * std::min(1.0, std::max(0.0, value)));
    return static_cast<unsigned char>(scaled);
}

// Blue for low, white for the middle of the range, red for high.
void divergingColor(double t, unsigned char* rgb) {
    t = std::min(1.0, std::max(-1.0, t));
    const double blue[3] = {0.23, 0.30, 0.75};
    const double red[3] = {0.71, 0.02, 0.15};
    for (int k = 0; k < 3; ++k) {
        const double target = t < 0.0 ? blue[k] : red[k];
        const double weight = std::fabs(t);
        rgb[k] = toByte((1.0 - weight) * 1.0 + weight * target);
    }
}

}  // namespace

bool writeFieldCsv(const std::string& path, const Field2D& field) {
    std::ofstream out(path);
    if (!out) {
        return false;
    }
    out.precision(9);
    for (int j = 0; j < field.ny(); ++j) {
        for (int i = 0; i < field.nx(); ++i) {
            if (i > 0) {
                out << ',';
            }
            out << field(i, j);
        }
        out << '\n';
    }
    return out.good();
}

bool writeVelocityCsv(const std::string& path, const StaggeredGrid& grid, const Field2D& u, const Field2D& v) {
    std::ofstream out(path);
    if (!out) {
        return false;
    }
    out.precision(9);
    out << "x,y,u,v,speed\n";
    for (int j = 0; j < grid.ny; ++j) {
        for (int i = 0; i < grid.nx; ++i) {
            const double uc = 0.5 * (u(i, j) + u(i + 1, j));
            const double vc = 0.5 * (v(i, j) + v(i, j + 1));
            out << grid.xP(i) << ',' << grid.yP(j) << ',' << uc << ',' << vc << ',' << std::hypot(uc, vc) << '\n';
        }
    }
    return out.good();
}

bool writeColumnsCsv(const std::string& path, const std::vector<std::string>& headers,
                     const std::vector<std::vector<double>>& columns) {
    assert(headers.size() == columns.size());
    std::ofstream out(path);
    if (!out) {
        return false;
    }
    out.precision(9);
    for (size_t k = 0; k < headers.size(); ++k) {
        out << (k > 0 ? "," : "") << headers[k];
    }
    out << '\n';

    size_t rows = 0;
    for (const std::vector<double>& column : columns) {
        rows = std::max(rows, column.size());
    }
    for (size_t row = 0; row < rows; ++row) {
        for (size_t k = 0; k < columns.size(); ++k) {
            if (k > 0) {
                out << ',';
            }
            if (row < columns[k].size()) {
                out << columns[k][row];
            }
        }
        out << '\n';
    }
    return out.good();
}

bool writePpm(const std::string& path, const Field2D& field, double low, double high, const Field2D* mask) {
    std::ofstream out(path, std::ios::binary);
    if (!out) {
        return false;
    }
    const int width = field.nx();
    const int height = field.ny();
    out << "P6\n" << width << ' ' << height << "\n255\n";

    const double middle = 0.5 * (low + high);
    const double halfRange = 0.5 * (high - low);
    std::vector<unsigned char> row(static_cast<size_t>(width) * 3);
    for (int j = height - 1; j >= 0; --j) {  // PPM rows run top to bottom
        for (int i = 0; i < width; ++i) {
            unsigned char* pixel = &row[static_cast<size_t>(i) * 3];
            if (mask != nullptr && (*mask)(i, j) != 0.0) {
                pixel[0] = pixel[1] = pixel[2] = 110;
                continue;
            }
            const double t = halfRange > 0.0 ? (field(i, j) - middle) / halfRange : 0.0;
            divergingColor(t, pixel);
        }
        out.write(reinterpret_cast<const char*>(row.data()), static_cast<std::streamsize>(row.size()));
    }
    return out.good();
}

}  // namespace navierStokes
