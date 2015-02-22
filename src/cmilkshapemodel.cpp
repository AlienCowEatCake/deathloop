/* 
   Copyright (C) 2011-2014,
        Mikhail Alexandrov  <alexandroff.m@gmail.com>
        Andrey Kurochkin    <andy-717@yandex.ru>
        Peter Zhigalov      <peter.zhigalov@gmail.com>

   This file is part of the `deathloop' program.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QFile>
#include <QByteArray>
#include "cmodel.h"

using namespace std;
//конструктор модели
CMilkshapeModel::CMilkshapeModel()
{
    m_numMeshes = 0;
    m_pMeshes = NULL;
    m_numMaterials = 0;
    m_pMaterials = NULL;
    m_numTriangles = 0;
    m_pTriangles = NULL;
    m_numVertices = 0;
    m_pVertices = NULL;
}

//деструктор модели
CMilkshapeModel::~CMilkshapeModel()
{
    for(int i = 0; i < m_numMeshes; i++)
        delete [] m_pMeshes[i].m_pTriangleIndices;
    for(int i = 0; i < m_numMaterials; i++)
        delete [] m_pMaterials[i].m_pTextureFilename;

    m_numMeshes = 0;
    if(m_pMeshes)
    {
        delete [] m_pMeshes;
        m_pMeshes = NULL;
    }

    m_numMaterials = 0;
    if(m_pMaterials)
    {
        delete [] m_pMaterials;
        m_pMaterials = NULL;
    }

    m_numTriangles = 0;
    if(m_pTriangles)
    {
        delete [] m_pTriangles;
        m_pTriangles = NULL;
    }

    m_numVertices = 0;
    if(m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
    }
}

// MilkShape 3D структуры

// упаковка структур (отключение выравнивания областей памяти)
#if defined(_MSC_VER)
#	pragma pack(push, packing)
#	pragma pack(1)
#	define PACK_STRUCT
#elif defined(__GNUC__) && __GNUC__ >= 4
#	pragma pack(push, 1)
#	define PACK_STRUCT
#elif defined(__GNUC__)
#	define PACK_STRUCT	__attribute__((packed))
#else
#	error you must byte-align these structures with the appropriate compiler directives
#endif

// определение типов данных
typedef unsigned char byte;
typedef unsigned short word;

// заголовок файла ms3d
struct MS3DHeader
{
    char m_ID[10];
    int m_version;
} PACK_STRUCT;

// данные о вершинах
struct MS3DVertex
{
    byte m_flags;
    float m_vertex[3];
    char m_boneID;
    byte m_refCount;
} PACK_STRUCT;

// данные о треугольниках
struct MS3DTriangle
{
    word m_flags;
    word m_vertexIndices[3];
    float m_vertexNormals[3][3];
    float m_s[3], m_t[3];
    byte m_smoothingGroup;
    byte m_groupIndex;
} PACK_STRUCT;

// данные о материалах
struct MS3DMaterial
{
    char m_name[32];
    float m_ambient[4];
    float m_diffuse[4];
    float m_specular[4];
    float m_emissive[4];
    float m_shininess;
    float m_transparency;
    byte m_mode;
    char m_texture[128];
    char m_alphamap[128];
} PACK_STRUCT;

// данные об анимации
struct MS3DJoint
{
    byte m_flags;
    char m_name[32];
    char m_parentName[32];
    float m_rotation[3];
    float m_translation[3];
    word m_numRotationKeyframes;
    word m_numTranslationKeyframes;
} PACK_STRUCT;

struct MS3DKeyframe
{
    float m_time;
    float m_parameter[3];
} PACK_STRUCT;

// конец упаковки структур
#if defined(_MSC_VER)
#	pragma pack(pop, packing)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	pragma pack(pop)
#endif
#undef PACK_STRUCT

// загрузка модели
bool CMilkshapeModel::loadModelData(const char *filename)
{
    QFile inputFile(filename);
    inputFile.open(QFile::ReadOnly);
    inputFile.seek(0);
    QByteArray pBuffer = inputFile.readAll();
    inputFile.close();

    const char *pPtr = pBuffer;
    MS3DHeader *pHeader = (MS3DHeader*)pPtr;
    pPtr += sizeof(MS3DHeader);

    if(strncmp(pHeader->m_ID, "MS3D000000", 10) != 0)
        return false; // неправильный заголовок файла
    // загрузка вершин
    int nVertices = *(word*)pPtr;
    m_numVertices = nVertices;
    m_pVertices = new Vertex[nVertices];
    pPtr += sizeof(word);

    int i;
    for(i = 0; i < nVertices; i++)
    {
        MS3DVertex *pVertex = (MS3DVertex*)pPtr;
        m_pVertices[i].m_boneID = pVertex->m_boneID;
        memcpy(m_pVertices[i].m_location, pVertex->m_vertex, sizeof(float)*3);
        pPtr += sizeof(MS3DVertex);
    }
    // загрузка треугольников
    int nTriangles = *(word*)pPtr;
    m_numTriangles = nTriangles;
    m_pTriangles = new Triangle[nTriangles];
    pPtr += sizeof(word);

    for(i = 0; i < nTriangles; i++)
    {
        MS3DTriangle *pTriangle = (MS3DTriangle*)pPtr;
        int vertexIndices[3] = {pTriangle->m_vertexIndices[0], pTriangle->m_vertexIndices[1], pTriangle->m_vertexIndices[2]};
        float t[3] = {1.0f-pTriangle->m_t[0], 1.0f-pTriangle->m_t[1], 1.0f-pTriangle->m_t[2]};
        memcpy(m_pTriangles[i].m_vertexNormals, pTriangle->m_vertexNormals, sizeof(float)*3*3);
        memcpy(m_pTriangles[i].m_s, pTriangle->m_s, sizeof(float)*3);
        memcpy(m_pTriangles[i].m_t, t, sizeof(float)*3);
        memcpy(m_pTriangles[i].m_vertexIndices, vertexIndices, sizeof(int)*3);
        pPtr += sizeof(MS3DTriangle);
    }
    // загрузка сетки
    int nGroups = *(word*)pPtr;
    m_numMeshes = nGroups;
    m_pMeshes = new Mesh[nGroups];
    pPtr += sizeof(word);
    for(i = 0; i < nGroups; i++)
    {
        pPtr += sizeof(byte);
        pPtr += 32;

        word nTriangles = *(word*)pPtr;
        pPtr += sizeof(word);
        int *pTriangleIndices = new int[nTriangles];
        for(int j = 0; j < nTriangles; j++)
        {
            pTriangleIndices[j] = *(word*)pPtr;
            pPtr += sizeof(word);
        }

        char materialIndex = *(char*)pPtr;
        pPtr += sizeof(char);

        m_pMeshes[i].m_materialIndex = materialIndex;
        m_pMeshes[i].m_numTriangles = nTriangles;
        m_pMeshes[i].m_pTriangleIndices = pTriangleIndices;
    }
    // загрузка материалов
    int nMaterials = *(word*)pPtr;
    m_numMaterials = nMaterials;
    m_pMaterials = new Material[nMaterials];
    pPtr += sizeof(word);
    for(i = 0; i < nMaterials; i++)
    {
        MS3DMaterial *pMaterial = (MS3DMaterial*)pPtr;
        memcpy(m_pMaterials[i].m_ambient, pMaterial->m_ambient, sizeof(float)*4);
        memcpy(m_pMaterials[i].m_diffuse, pMaterial->m_diffuse, sizeof(float)*4);
        memcpy(m_pMaterials[i].m_specular, pMaterial->m_specular, sizeof(float)*4);
        memcpy(m_pMaterials[i].m_emissive, pMaterial->m_emissive, sizeof(float)*4);
        m_pMaterials[i].m_shininess = pMaterial->m_shininess;
        m_pMaterials[i].m_pTextureFilename = new char[7+strlen(pMaterial->m_texture)+1];
        strcpy(m_pMaterials[i].m_pTextureFilename, ":/data/");
        strcpy(m_pMaterials[i].m_pTextureFilename+7, pMaterial->m_texture);
        /* пофикшено в самих модельках, но let it be
        // При компиляции используются текстуры в PNG-8 для экономии места
        strcpy(m_pMaterials[i].m_pTextureFilename+7+strlen(pMaterial->m_texture)-3, "png");
        */
        pPtr += sizeof(MS3DMaterial);
    }

    reloadTextures();
    pBuffer.clear();
    return true;
}

// просчет нормалей
void CMilkshapeModel::normal_gen()
{
    for(int i = 0; i < m_numTriangles; i++)
    {
        float ax = m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[0] - m_pVertices[m_pTriangles[i].m_vertexIndices[0]].m_location[0];
        float ay = m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[1] - m_pVertices[m_pTriangles[i].m_vertexIndices[0]].m_location[1];
        float az = m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[2] - m_pVertices[m_pTriangles[i].m_vertexIndices[0]].m_location[2];
        float bx = m_pVertices[m_pTriangles[i].m_vertexIndices[2]].m_location[0] - m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[0];
        float by = m_pVertices[m_pTriangles[i].m_vertexIndices[2]].m_location[1] - m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[1];
        float bz = m_pVertices[m_pTriangles[i].m_vertexIndices[2]].m_location[2] - m_pVertices[m_pTriangles[i].m_vertexIndices[1]].m_location[2];
        float nx = ay * bz - by * az;
        float ny = az * bx - bz * ax;
        float nz = ax * by - bx * ay;
        float nn = sqrt(nx * nx + ny * ny + nz * nz);
        for(int j = 0; j < 3; j++)
        {
            m_pTriangles[i].m_vertexNormals[j][0] = nx / nn;
            m_pTriangles[i].m_vertexNormals[j][1] = ny / nn;
            m_pTriangles[i].m_vertexNormals[j][2] = nz / nn;
        }
    }
    // сглаживание нормалей
    for(int i = 0; i < m_numVertices; i++)
    {
        int counter = 0;
        float count_x = 0.0;
        float count_y = 0.0;
        float count_z = 0.0;
        for(int j = 0; j < m_numTriangles; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                if(m_pTriangles[j].m_vertexIndices[k] == i)
                {
                    count_x += m_pTriangles[j].m_vertexNormals[k][0];
                    count_y += m_pTriangles[j].m_vertexNormals[k][1];
                    count_z += m_pTriangles[j].m_vertexNormals[k][2];
                    counter++;
                }
            }
        }
        count_x = count_x / (float)counter;
        count_y = count_y / (float)counter;
        count_z = count_z / (float)counter;
        for(int j = 0; j < m_numTriangles; j++)
        {
            for(int k = 0; k < 3; k++)
            {
                if(m_pTriangles[j].m_vertexIndices[k] == i)
                {
                    m_pTriangles[j].m_vertexNormals[k][0] = (count_x + 0.35 * m_pTriangles[j].m_vertexNormals[k][0]) / 1.35;
                    m_pTriangles[j].m_vertexNormals[k][1] = (count_y + 0.35 * m_pTriangles[j].m_vertexNormals[k][1]) / 1.35;
                    m_pTriangles[j].m_vertexNormals[k][2] = (count_z + 0.35 * m_pTriangles[j].m_vertexNormals[k][2]) / 1.35;
                }
            }
        }
    }
}

//=============================================================================================================================
// класс наклонный участок
// конструктор модели
m_track_s1::m_track_s1()
{
    m_numMeshes = 0;
    m_pMeshes = NULL;
    m_numMaterials = 0;
    m_pMaterials = NULL;
    m_numTriangles = 0;
    m_pTriangles = NULL;
    m_numVertices = 0;
    m_pVertices = NULL;
    vert_backup = NULL;
    o_length = -1.0;
    o_beta = -1.0;
    o_rloop = -1.0;
}

// деструктор модели
m_track_s1::~m_track_s1()
{
    int i;
    for ( i = 0; i < m_numMeshes; i++ )
        delete[] m_pMeshes[i].m_pTriangleIndices;
    for ( i = 0; i < m_numMaterials; i++ )
        delete[] m_pMaterials[i].m_pTextureFilename;
    m_numMeshes = 0;
    if ( m_pMeshes != NULL )
    {
        delete[] m_pMeshes;
        m_pMeshes = NULL;
    }
    m_numMaterials = 0;
    if ( m_pMaterials != NULL )
    {
        delete[] m_pMaterials;
        m_pMaterials = NULL;
    }
    m_numTriangles = 0;
    if ( m_pTriangles != NULL )
    {
        delete[] m_pTriangles;
        m_pTriangles = NULL;
    }
    m_numVertices = 0;
    if ( m_pVertices != NULL )
    {
        delete[] m_pVertices;
        m_pVertices = NULL;
    }
    if ( vert_backup != NULL )
    {
        delete[] vert_backup;
        vert_backup = NULL;
    }
}

// изменение размеров модели
void m_track_s1::resize(float length, float beta, float rloop)
{
    //if(o_length != length || o_beta != beta || o_rloop != rloop)
    if(fabs((o_length - length) / length) > 0.001 || fabs((o_beta - beta) / beta) > 0.001 || fabs((o_rloop - rloop) / rloop) > 0.001)
    {
        //копия вершин
        if(!vert_backup)
        {
            vert_backup = new Vertex [m_numVertices];
            for(int i = 0; i < m_numVertices; i++)
            {
                vert_backup[i].m_location[0] = m_pVertices[i].m_location[0];
                vert_backup[i].m_location[1] = m_pVertices[i].m_location[1];
                vert_backup[i].m_location[2] = m_pVertices[i].m_location[2];
            }
        }
        //сохранение параметров
        o_beta = beta;
        o_length = length;
        o_rloop = rloop;
        //изменение размеров
        float epsilon = 0.5;
        float tmp1 = rloop / def_rloop;
        float tmp2 = maction.RSph * sqrt(2.0) * sin(maction.Beta);
        for(int i = 0; i < this->m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = vert_backup[i].m_location[0] * /*rloop / def_rloop*/ tmp1;
            m_pVertices[i].m_location[2] = (vert_backup[i].m_location[2] - def_dz) * /*rloop / def_rloop*/ tmp1;
            if(fabs(vert_backup[i].m_location[1]) > epsilon)
            {
                m_pVertices[i].m_location[1] = vert_backup[i].m_location[1] * length + /*maction.RSph * sqrt(2.0) * sin(maction.Beta)*/ tmp2;
            }
            else
            {
                m_pVertices[i].m_location[1] = vert_backup[i].m_location[1];
            }
        }
        //поворот осей координат
        tmp1 = cos(beta - M_PI_2);
        tmp2 = sin(beta - M_PI_2);
        for(int i = 0; i < this->m_numVertices; i++)
        {
            float old_location_x = m_pVertices[i].m_location[0], old_location_y = m_pVertices[i].m_location[1];
            /*
            m_pVertices[i].m_location[0] = old_location_x * cos(beta - M_PI_2) + old_location_y * sin(beta - M_PI_2);
            m_pVertices[i].m_location[1] = - old_location_x * sin(beta - M_PI_2) + old_location_y * cos(beta - M_PI_2);
            */
            m_pVertices[i].m_location[0] = old_location_x * tmp1 + old_location_y * tmp2;
            m_pVertices[i].m_location[1] = - old_location_x * tmp2 + old_location_y * tmp1;
        }
        //сдвиг осей координат
        tmp1 = rloop * sin(beta);
        tmp2 = rloop * cos(beta);
        for(int i = 0; i < this->m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] /*+= - rloop * sin(beta);*/ -= tmp1;
            m_pVertices[i].m_location[1] /*+= - rloop * cos(beta);*/ -= tmp2;
        }
        // просчет нормалей
        normal_gen();
    }
}

//=============================================================================================================================
// класс входной сектор в петлю
// конструктор модели
m_track_s2::m_track_s2()
{
    m_numMeshes = 0;
    m_pMeshes = NULL;
    m_numMaterials = 0;
    m_pMaterials = NULL;
    m_numTriangles = 0;
    m_pTriangles = NULL;
    m_numVertices = 0;
    m_pVertices = NULL;
    vert_backup = NULL;
    o_radius = -1.0;
    o_beta = -1.0;
}

// деструктор модели
m_track_s2::~m_track_s2()
{
    int i;
    for ( i = 0; i < m_numMeshes; i++ )
        delete[] m_pMeshes[i].m_pTriangleIndices;
    for ( i = 0; i < m_numMaterials; i++ )
        delete[] m_pMaterials[i].m_pTextureFilename;

    m_numMeshes = 0;
    if ( m_pMeshes != NULL )
    {
        delete[] m_pMeshes;
        m_pMeshes = NULL;
    }

    m_numMaterials = 0;
    if ( m_pMaterials != NULL )
    {
        delete[] m_pMaterials;
        m_pMaterials = NULL;
    }

    m_numTriangles = 0;
    if ( m_pTriangles != NULL )
    {
        delete[] m_pTriangles;
        m_pTriangles = NULL;
    }

    m_numVertices = 0;
    if ( m_pVertices != NULL )
    {
        delete[] m_pVertices;
        m_pVertices = NULL;
    }

    if ( vert_backup != NULL )
    {
        delete[] vert_backup;
        vert_backup = NULL;
    }
}

// изменение размеров модели
void m_track_s2::resize(float radius, float beta)
{
    //if(radius != o_radius || beta != o_beta)
    if(fabs((o_radius - radius) / radius) > 0.001 || fabs((o_beta - beta) / beta) > 0.001)
    {
        //копия вершин
        if(!vert_backup)
        {
            vert_backup = new Vertex [m_numVertices];
            for(int i = 0; i < m_numVertices; i++)
            {
                vert_backup[i].m_location[0] = m_pVertices[i].m_location[0];
                vert_backup[i].m_location[1] = m_pVertices[i].m_location[1];
                vert_backup[i].m_location[2] = m_pVertices[i].m_location[2];
            }
        }
        //сохранение параметров
        o_beta = beta;
        o_radius = radius;
        //задание радиуса
        float tmp1 = radius / def_rloop;
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = vert_backup[i].m_location[0] * /*radius / def_rloop*/ tmp1;
            m_pVertices[i].m_location[1] = vert_backup[i].m_location[1] * /*radius / def_rloop*/ tmp1;
            m_pVertices[i].m_location[2] = vert_backup[i].m_location[2] * /*radius / def_rloop*/ tmp1;
        }
        //сжатие
        for(int i = 0; i < this->m_numVertices; i++)
        {
            convert_point(beta, m_pVertices[i].m_location[0], m_pVertices[i].m_location[1],
                          &(m_pVertices[i].m_location[0]), &(m_pVertices[i].m_location[1]));
        }
        // просчет нормалей
        normal_gen();
    }
}

//=============================================================================================================================
// класс петля
// конструктор модели
m_track_s3::m_track_s3()
{
    m_numMeshes = 0;
    m_pMeshes = NULL;
    m_numMaterials = 0;
    m_pMaterials = NULL;
    m_numTriangles = 0;
    m_pTriangles = NULL;
    m_numVertices = 0;
    m_pVertices = NULL;
    vert_backup = NULL;
    o_radius = -1.0;
}

// деструктор модели
m_track_s3::~m_track_s3()
{
    int i;
    for ( i = 0; i < m_numMeshes; i++ )
        delete[] m_pMeshes[i].m_pTriangleIndices;
    for ( i = 0; i < m_numMaterials; i++ )
        delete[] m_pMaterials[i].m_pTextureFilename;

    m_numMeshes = 0;
    if ( m_pMeshes != NULL )
    {
        delete[] m_pMeshes;
        m_pMeshes = NULL;
    }

    m_numMaterials = 0;
    if ( m_pMaterials != NULL )
    {
        delete[] m_pMaterials;
        m_pMaterials = NULL;
    }

    m_numTriangles = 0;
    if ( m_pTriangles != NULL )
    {
        delete[] m_pTriangles;
        m_pTriangles = NULL;
    }

    m_numVertices = 0;
    if ( m_pVertices != NULL )
    {
        delete[] m_pVertices;
        m_pVertices = NULL;
    }

    if ( vert_backup != NULL )
    {
        delete[] vert_backup;
        vert_backup = NULL;
    }
}

// изменение размеров модели
void m_track_s3::resize(float radius)
{
    //if(radius != o_radius)
    if(fabs((o_radius - radius) / radius) > 0.001)
    {
        //копия вершин
        if(!vert_backup)
        {
            vert_backup = new Vertex [m_numVertices];
            for(int i = 0; i < m_numVertices; i++)
            {
                vert_backup[i].m_location[0] = m_pVertices[i].m_location[0];
                vert_backup[i].m_location[1] = m_pVertices[i].m_location[1];
                vert_backup[i].m_location[2] = m_pVertices[i].m_location[2];
            }
        }
        //сохранение параметров
        o_radius = radius;
        //задание радиуса
        float tmp1 = radius / def_rloop;
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = vert_backup[i].m_location[0] * /*radius / def_rloop*/ tmp1;
            m_pVertices[i].m_location[1] = vert_backup[i].m_location[1] * /*radius / def_rloop*/ tmp1;
            m_pVertices[i].m_location[2] = vert_backup[i].m_location[2] * /*radius / def_rloop*/ tmp1;
        }
        // просчет нормалей
        normal_gen();
    }
}

//=============================================================================================================================
// класс горизонтальный участок желоба
// конструктор модели
m_track_s4::m_track_s4()
{
    m_numMeshes = 0;
    m_pMeshes = NULL;
    m_numMaterials = 0;
    m_pMaterials = NULL;
    m_numTriangles = 0;
    m_pTriangles = NULL;
    m_numVertices = 0;
    m_pVertices = NULL;
    vert_backup = NULL;
    o_length = -1.0;
    o_rloop = -1.0;
}

// деструктор модели
m_track_s4::~m_track_s4()
{
    int i;
    for ( i = 0; i < m_numMeshes; i++ )
        delete[] m_pMeshes[i].m_pTriangleIndices;
    for ( i = 0; i < m_numMaterials; i++ )
        delete[] m_pMaterials[i].m_pTextureFilename;

    m_numMeshes = 0;
    if ( m_pMeshes != NULL )
    {
        delete[] m_pMeshes;
        m_pMeshes = NULL;
    }

    m_numMaterials = 0;
    if ( m_pMaterials != NULL )
    {
        delete[] m_pMaterials;
        m_pMaterials = NULL;
    }

    m_numTriangles = 0;
    if ( m_pTriangles != NULL )
    {
        delete[] m_pTriangles;
        m_pTriangles = NULL;
    }

    m_numVertices = 0;
    if ( m_pVertices != NULL )
    {
        delete[] m_pVertices;
        m_pVertices = NULL;
    }

    if ( vert_backup != NULL )
    {
        delete[] vert_backup;
        vert_backup = NULL;
    }
}

// изменение размеров модели
void m_track_s4::resize(float length, float rloop)
{
    //if(length != o_length || rloop != o_rloop)
    if(fabs((o_length - length) / length) > 0.001 || fabs((o_rloop - rloop) / rloop) > 0.001)
    {
        //копия вершин
        if(!vert_backup)
        {
            vert_backup = new Vertex [m_numVertices];
            for(int i = 0; i < m_numVertices; i++)
            {
                vert_backup[i].m_location[0] = m_pVertices[i].m_location[0];
                vert_backup[i].m_location[1] = m_pVertices[i].m_location[1];
                vert_backup[i].m_location[2] = m_pVertices[i].m_location[2];
            }
        }
        //сохранение параметров
        o_rloop = rloop;
        o_length = length;
        //изменение размеров
        float epsilon = 0.5;
        float tmp1 = rloop / def_rloop;
        for(int i = 0; i < this->m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = vert_backup[i].m_location[0] * /*rloop / def_rloop*/ tmp1;
            m_pVertices[i].m_location[2] = (vert_backup[i].m_location[2] + def_dz) * /*rloop / def_rloop*/ tmp1;
            if(fabs(vert_backup[i].m_location[1]) > epsilon)
            {
                m_pVertices[i].m_location[1] = vert_backup[i].m_location[1] * length;
            }
            else
            {
                m_pVertices[i].m_location[1] = vert_backup[i].m_location[1];
            }
        }
        //поворот осей координат
        for(int i = 0; i < this->m_numVertices; i++)
        {

            float old_location_x = m_pVertices[i].m_location[0], old_location_y = m_pVertices[i].m_location[1];
            /*
            m_pVertices[i].m_location[0] = old_location_x * cos(- M_PI_2) + old_location_y * sin(- M_PI_2);
            m_pVertices[i].m_location[1] = - old_location_x * sin(- M_PI_2) + old_location_y * cos(- M_PI_2);
            */
            m_pVertices[i].m_location[0] = - old_location_y;
            m_pVertices[i].m_location[1] = old_location_x;
        }
        //сдвиг осей координат
        for(int i = 0; i < this->m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] += length;
            m_pVertices[i].m_location[1] -= rloop;
        }
        // просчет нормалей
        normal_gen();
    }
}

//=============================================================================================================================
// класс шарик
// конструктор модели
m_sphere::m_sphere()
{
    m_numMeshes = 0;
    m_pMeshes = NULL;
    m_numMaterials = 0;
    m_pMaterials = NULL;
    m_numTriangles = 0;
    m_pTriangles = NULL;
    m_numVertices = 0;
    m_pVertices = NULL;
    vert_backup = NULL;
    o_radius = -1.0;
}

// деструктор модели
m_sphere::~m_sphere()
{
    int i;
    for ( i = 0; i < m_numMeshes; i++ )
        delete[] m_pMeshes[i].m_pTriangleIndices;
    for ( i = 0; i < m_numMaterials; i++ )
        delete[] m_pMaterials[i].m_pTextureFilename;

    m_numMeshes = 0;
    if ( m_pMeshes != NULL )
    {
        delete[] m_pMeshes;
        m_pMeshes = NULL;
    }

    m_numMaterials = 0;
    if ( m_pMaterials != NULL )
    {
        delete[] m_pMaterials;
        m_pMaterials = NULL;
    }

    m_numTriangles = 0;
    if ( m_pTriangles != NULL )
    {
        delete[] m_pTriangles;
        m_pTriangles = NULL;
    }

    m_numVertices = 0;
    if ( m_pVertices != NULL )
    {
        delete[] m_pVertices;
        m_pVertices = NULL;
    }

    if ( vert_backup != NULL )
    {
        delete[] vert_backup;
        vert_backup = NULL;
    }
}

// изменение размеров модели
void m_sphere::resize(float radius)
{
    //if(o_radius != radius)
    if(fabs((o_radius - radius) / radius) > 0.001)
    {
        //копия вершин
        if(!vert_backup)
        {
            vert_backup = new Vertex [m_numVertices];
            for(int i = 0; i < m_numVertices; i++)
            {
                vert_backup[i].m_location[0] = m_pVertices[i].m_location[0];
                vert_backup[i].m_location[1] = m_pVertices[i].m_location[1];
                vert_backup[i].m_location[2] = m_pVertices[i].m_location[2];
            }
        }
        //сохранение параметров
        o_radius = radius;
        //задание радиуса
        float tmp1 = radius / def_rsph;
        for(int i = 0; i < m_numVertices; i++)
        {
            m_pVertices[i].m_location[0] = vert_backup[i].m_location[0] * /*radius / def_rsph*/ tmp1;
            m_pVertices[i].m_location[1] = vert_backup[i].m_location[1] * /*radius / def_rsph*/ tmp1;
            m_pVertices[i].m_location[2] = vert_backup[i].m_location[2] * /*radius / def_rsph*/ tmp1;
        }
        // просчет нормалей
        normal_gen();
    }
}

