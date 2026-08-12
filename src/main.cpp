#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct MapData
{
    int width = 0;
    int height = 0;
    int tileSize = 16;
    std::vector<int> tiles;
};

static std::string readFile(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Could not open file: " + path.string());

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

static std::string extractAttribute(const std::string& xml, const std::string& tagName, const std::string& attribute)
{
    const std::regex pattern("<" + tagName + "[^>]*\\b" + attribute + "=\"([^\"]+)\"[^>]*>");
    std::smatch match;
    if (std::regex_search(xml, match, pattern))
        return match[1].str();

    throw std::runtime_error("Missing attribute '" + attribute + "' on tag '<" + tagName + ">'");
}

static MapData loadCastleMap(const std::filesystem::path& mapPath)
{
    const std::string xml = readFile(mapPath);

    const std::string widthText = extractAttribute(xml, "map", "width");
    const std::string heightText = extractAttribute(xml, "map", "height");
    const std::string tileWidthText = extractAttribute(xml, "map", "tilewidth");
    const std::string tileHeightText = extractAttribute(xml, "map", "tileheight");

    const int width = std::stoi(widthText);
    const int height = std::stoi(heightText);
    const int tileSize = std::stoi(tileWidthText);

    if (tileSize != std::stoi(tileHeightText))
        throw std::runtime_error("Map tile width and height differ, which is not supported by this quick renderer.");

    const std::size_t dataStart = xml.find("<data encoding=\"csv\">");
    if (dataStart == std::string::npos)
        throw std::runtime_error("CSV layer data was not found in the castle map.");

    const std::size_t dataEnd = xml.find("</data>", dataStart);
    if (dataEnd == std::string::npos)
        throw std::runtime_error("CSV layer data is missing its closing tag.");

    const std::string csv = xml.substr(dataStart + std::string("<data encoding=\"csv\">").length(), dataEnd - dataStart - std::string("<data encoding=\"csv\">").length());

    MapData map;
    map.width = width;
    map.height = height;
    map.tileSize = tileSize;

    std::stringstream stream(csv);
    std::string row;
    while (std::getline(stream, row, '\n'))
    {
        std::stringstream rowStream(row);
        std::string value;
        while (std::getline(rowStream, value, ','))
        {
            const std::string cleaned = value;
            if (cleaned.empty() || cleaned == "\r")
                continue;

            map.tiles.push_back(std::stoi(cleaned));
        }
    }

    if (static_cast<int>(map.tiles.size()) != width * height)
        throw std::runtime_error("Map tile count does not match width * height.");

    return map;
}

int main()
{
    try
    {
        const std::filesystem::path mapPath = "maps/castle_d_1.tmx";
        const std::filesystem::path tilesetPath = "maps/tilesets/castle_dung_ts.png";

        const MapData map = loadCastleMap(mapPath);

        sf::Texture tilesetTexture;
        if (!tilesetTexture.loadFromFile(tilesetPath))
            throw std::runtime_error("Could not load tileset image: " + tilesetPath.string());

        sf::RenderWindow window(sf::VideoMode({800, 600}), "Final Fantasy Next - Castle Map");

        sf::View view(sf::FloatRect({0.f, 0.f}, {static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)}));
        window.setView(view);

        const int columns = 8;

        while (window.isOpen())
        {
            while (auto event = window.pollEvent())
            {
                if (event->is<sf::Event::Closed>())
                    window.close();
            }

            window.clear(sf::Color::Black);

            for (int y = 0; y < map.height; ++y)
            {
                for (int x = 0; x < map.width; ++x)
                {
                    const int tileIndex = map.tiles[static_cast<std::size_t>(y * map.width + x)];
                    if (tileIndex <= 0)
                        continue;

                    const int tilesetIndex = tileIndex - 1;
                    const int tileX = (tilesetIndex % columns) * map.tileSize;
                    const int tileY = (tilesetIndex / columns) * map.tileSize;

                    sf::Sprite sprite(tilesetTexture, sf::IntRect({tileX, tileY}, {map.tileSize, map.tileSize}));
                    sprite.setPosition({static_cast<float>(x * map.tileSize), static_cast<float>(y * map.tileSize)});
                    window.draw(sprite);
                }
            }

            window.display();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
