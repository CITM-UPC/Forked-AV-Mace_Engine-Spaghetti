#ifndef __RESOURCE_IMPORTER_H__
#define __RESOURCE_IMPORTER_H__

#include <string>
#include <vector>

// Clase para manejar la importación de recursos
class ResourceImporter {
public:
    // Convierte un modelo OBJ en un formato optimizado para el motor
    static bool ConvertOBJToLibrary(const std::string& inputPath, const std::string& outputPath);
};

#endif // __RESOURCE_IMPORTER_H__
