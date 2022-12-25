// ---------------------------------------------------------------------------------------------------------------------------------
//   _____                      _____  ____                       
//  / ____|                    |  __ \|  _ \                      
// | |  __  ___  ___  _ __ ___ | |  | | |_) |     ___ _ __  _ __  
// | | |_ |/ _ \/ _ \| '_ ` _ \| |  | |  _ <     / __| '_ \| '_ \ 
// | |__| |  __/ (_) | | | | | | |__| | |_) | _ | (__| |_) | |_) |
//  \_____|\___|\___/|_| |_| |_|_____/|____/ (_) \___| .__/| .__/ 
//                                                   | |   | |    
//                                                   |_|   |_|    
//
// Description:
//
//   Geometry database manager
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/03/2001 by Paul Nettle: Original creation
//
// Restrictions & freedoms pertaining to usage and redistribution of this software:
//
//   This software is 100% free. If you use this software (in part or in whole) you must credit the author. This software may not be
//   re-distributed (in part or in whole) in a modified form without clear documentation on how to obtain a copy of the original
//   work. You may not use this software to directly or indirectly cause harm to others. This software is provided as-is and without
//   warrantee -- Use at your own risk. For more information, visit HTTP://www.FluidStudios.com/
//
// Copyright 2002, Fluid Studios, Inc., all rights reserved.
// ---------------------------------------------------------------------------------------------------------------------------------

#include "Precomp.h"

#ifdef ACTIVE_EDITORS

#include "GeomDB.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

	GeomDB::GeomDB()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------

	GeomDB::~GeomDB()
{
}

// ---------------------------------------------------------------------------------------------------------------------------------


bool GeomDB::ReadGeomety(const geom::Color3 & defaultReflectivity)
{
    try
    {
        // DJ_TEMP : prog.setCurrentStatus("Loading geometry");

        // DJ_TEMP : prog.setCurrentStatus("Converting geometry");

        unsigned int	polyID = 0;
        unsigned int	totalPolys = 0;

        // count total number of triangles
        // DJ_TEMP : 
        /*
        // Pre-count stuff
        {
            for (unsigned int i = 0; i < ase.meshes().size(); ++i)
            {
                totalPolys += ase.meshes()[i].faces.size() / 3;
            }
        }
        */

        // Reserve for speed

        lightmaps().reserve(totalPolys);
        polys().reserve(totalPolys);
        RadLMap	emptyLightmap(128, 128, static_cast<int>(polyID));

        // for all the elements
        // DJ_TEMP : 
        //for (unsigned int i = 0; i < ase.meshes().size(); ++i)
        for (unsigned int i = 0; i < 1; ++i)
        {
            // for all the triangles of that element
            // DJ_TEMP : for (unsigned int j = 0; j < obj.faces.size(); j += 3, ++polyID)
            {
                // DJ_TEMP : 
                /*
                if (!(polyID&0x3))
                {
                    prog.setCurrentPercent(static_cast<float>(polyID) / static_cast<float>(totalPolys) * 100.0f);
                    if (prog.stopRequested()) throw "";
                }
                */

                // Vertex indices

                RadPrim		poly;

                // Vertices
                // get the 3 vertices for that triangle
                // DJ_TEMP
                /*
                geom::Point3	p0 = obj.verts[obj.faces[a]];
                geom::Point3	p1 = obj.verts[obj.faces[b]];
                geom::Point3	p2 = obj.verts[obj.faces[c]];
                */

                // Textured vertices
                // get the uvs for the diffuse mapping
                // DJ_TEMP
                /*
                geom::Point2	t0(0, 0);
                geom::Point2	t1(0, 0);
                geom::Point2	t2(0, 0);
                */

                // Swap orientation because max uses non-left-handed coordinates
                // DJ_TEMP : necessary???
                /*
                {
                    // Get the normal

                    geom::Vector3	v = (p0-p1)%(p2-p1);

                    // Swap y/z

                    fstl::swap(p0.y(), p0.z());
                    fstl::swap(p1.y(), p1.z());
                    fstl::swap(p2.y(), p2.z());

                    // If the normal swapped, re-order

                    geom::Vector3	nv = (p0-p1)%(p2-p1);
                    if (nv != v)
                    {
                        fstl::swap(p0, p1);
                        fstl::swap(t0, t1);
                    }
                }*/

                // build the triangle
                // DJ_TEMP : 
                /*
                poly.xyz() += p0;
                poly.xyz() += p1;
                poly.xyz() += p2;
                poly.uv() += geom::Point2(0,0);
                poly.uv() += geom::Point2(0,0);
                poly.uv() += geom::Point2(0,0);
                poly.texuv() += t0;
                poly.texuv() += t1;
                poly.texuv() += t2;
                poly.textureID() = i;
                poly.polyID() = i;
                */

                poly.calcPlane(false);

                poly.illuminationColor() = geom::Color3(0,0,0);

                // Is there a material? If so, use it for the reflective color

                geom::Color3	reflectivity = defaultReflectivity;

                // DJ_TEMP : if(material...)
                //if (ase.materials().size())
                {
                    // Default to the material's color

                    // DJ_TEMP : reflectivity = diffuseColor;
                }

                poly.reflectanceColor() = reflectivity;
                polys() += poly;
            }
        }

        // Convert the lights to patches

        {
            // DJ_TEMP : prog.setCurrentStatus("Converting lights");

            // We reserve for speed

            // DJ_TEMP : list of lights for radiosity : lights().reserve(ase.lights().size());

            // DJ_TEMP : for all lights in lightlist
            //for (unsigned int i = 0; i < ase.lights().size(); i++)
            {
                // DJ_TEMP : prog.setCurrentPercent(static_cast<float>(i) / static_cast<float>(ase.lights().size()) * 100.0f);
                // DJ_TEMP : if (prog.stopRequested()) throw false;
                
                // DJ_TEMP 
                // get position 
                // get intensity/color

                // Swap orientation because max uses non-left-handed coordinates
                //geom::Point3	pos = curLight.pos;
                //fstl::swap(pos.y(), pos.z());

                // create the two patch light (for omni) or only one (for directional)
                /*
                RadPatch	patch1;
                patch1.area() = 0;
                patch1.origin() = geom::Point3(pos.x(), pos.y(), pos.z());
                patch1.energy() = curLight.color * static_cast<float>(curLight.intensity);
                fstl::swap(patch1.energy().r(), patch1.energy().b());
                patch1.plane() = geom::Plane3(patch1.origin(), geom::Vector3(0, 1, 0));
                lights() += patch1;

                RadPatch	patch2;
                patch2.area() = 0;
                patch2.origin() = geom::Point3(pos.x(), pos.y(), pos.z());
                patch2.energy() = curLight.color * static_cast<float>(curLight.intensity);
                fstl::swap(patch1.energy().r(), patch2.energy().b());
                patch2.plane() = geom::Plane3(patch2.origin(), geom::Vector3(0, -1, 0));
                lights() += patch2;*/

            }
        }
    }
    catch (const char * err)
    {
        if (err && *err)
        {
            AfxMessageBox(err);
            return false;
        }
    }

    return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------
// GeomDB.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS