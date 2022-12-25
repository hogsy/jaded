// ---------------------------------------------------------------------------------------------------------------------------------
//  _____            _  _____                                 
// |  __ \          | |/ ____|                                
// | |__) | __ _  __| | |  __  ___ _ __       ___ _ __  _ __  
// |  _  / / _` |/ _` | | |_ |/ _ \ '_ \     / __| '_ \| '_ \ 
// | | \ \| (_| | (_| | |__| |  __/ | | | _ | (__| |_) | |_) |
// |_|  \_\\__,_|\__,_|\_____|\___|_| |_|(_) \___| .__/| .__/ 
//                                               | |   | |    
//                                               |_|   |_|    
//
// Description:
//
//   Radiosity generation main code
//
// Notes:
//
//   Best viewed with 8-character tabs and (at least) 132 columns
//
// History:
//
//   08/17/2001 by Paul Nettle: Original creation
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

#include "radprim.h"
#include "BSP.h"
#include "SOctree.h"
#include "RadGen.h"
#include "BeamTree.h"
#include "PolarQuadtree.h"
#include "LMapGen.h"

// ---------------------------------------------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------------------------------------------------------------------------------------------------------------

static	bool	_processEnergy(SOctree & node, void * userData)
{
	RadGen *	rg = static_cast<RadGen *>(userData);
	if (!rg) return false;

	return rg->processEnergy(node);
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::calcVisiblePieces(const RadPatch & emitter, const RadPrimPointerListGrainy & potentialReceivers, RadPrimListGrainy & visiblePieces)
{
#if 0
	// Insert the polygons into the polar QT for visibility

	geom::Plane3		axis = emitter.plane();
	axis.origin() = emitter.origin();
	PolarQuadtree		pqt(axis);

	// The potential receivers is an ordered list from back-to-front, so go through it backwards...

	for (RadPrimPointerListGrainy::node * i = potentialReceivers.tail(); i; i = i->prev())
	{
		pqt.insert(*i->data());
	}

	visiblePieces += pqt.visiblePieces();
#else
	// Insert the polygons into the beamtree for visibility

	BeamTree		bt;
	geom::Plane3Array	pa;
	pa += emitter.plane();
	bt.init(pa);
	geom::Plane3Array	planes;

	// The potential receivers is an ordered list from back-to-front, so go through it backwards...

	for (RadPrimPointerListGrainy::node * i = potentialReceivers.tail(); i; i = i->prev())
	{
		bt.insert(emitter.origin(), *i->data(), visiblePieces);
	}
#endif
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::emit(RadPatch & emitter, const RadPrimPointerListGrainy & potentialReceivers, RadLMapArray & storedEnergy)
{
	// What's visible from this emitter?

	RadPrimListGrainy	visiblePieces;
	calcVisiblePieces(emitter, potentialReceivers, visiblePieces);

	// Clear out the emitter's energy before we shoot. Theoretically, we never should need to do this, because an emitter
	// should never be able to emit energy onto itself, but it won't hurt to do this for completeness.

	geom::Color3	shootingEnergy = emitter.energy();
	geom::Color3	reflectedEnergy(0,0,0);
	geom::Color3	receivedEnergy(0,0,0);
	emitter.energy() = geom::Color3(0,0,0);

	// Light up the visible pieces
	{
		for (RadPrimListGrainy::node * i = visiblePieces.head(); i; i = i->next())
		{
			RadPrim &	prim = i->data();
			RadPrim &	originalPrim = *i->data().originalPrimitive();
			RadLMap &	se = storedEnergy[originalPrim.textureID()];

			prim.prepareNoPatches();
			unsigned int	fIndex = 0;

			float	totalArea = (prim.uXFormVector() % prim.vXFormVector()).length();

			geom::Point3	elementCenter = prim.minXYZ() + prim.uXFormVector() * 0.5 + prim.vXFormVector() * 0.5;

			unsigned int	minU = static_cast<unsigned int>(prim.minUV().u());
			unsigned int	minV = static_cast<unsigned int>(prim.minUV().v());
			unsigned int	maxU = static_cast<unsigned int>(prim.maxUV().u());
			unsigned int	maxV = static_cast<unsigned int>(prim.maxUV().v());

			unsigned int	ominU = static_cast<unsigned int>(originalPrim.minUV().u());
			unsigned int	omaxU = static_cast<unsigned int>(originalPrim.maxUV().u());
			unsigned int	ominV = static_cast<unsigned int>(originalPrim.minUV().v());

			for (unsigned int v = minV; v <= maxV; ++v)
			{
				geom::Point3	ec = elementCenter;
				for (unsigned int u = minU; u <= maxU; ++u, ++fIndex, ec += prim.uXFormVector())
				{
					// Receiver must have area

					float	area_j = prim.elementAreas()[fIndex];
					if (area_j <= 0) continue;

					// Ignore same planes

					if ((prim.plane().normal() ^ emitter.plane().normal()) > 0.009f) continue;

					// Get the whole area of this patch from the original polygon

					unsigned int	pu = u - ominU;
					unsigned int	pv = v - ominV;
					unsigned int	elementIndex = pv * (omaxU-ominU+1) + pu;

					// Here we calculate the radiosity equation for a differential area form factor.
					// It looks like this:
					//
					// Fij = (cos(Theta_i) * cos(Theta_j)) / (PI * distanceSquared) * Hij * dAj
					//
					// Where:
					//	Fij is the form factor from the transmission patch (i) to the receiving patch (j)
					//	theta_i is the angle between the normal of (i) and the direction from (i) to (j)
					//	theta_j is the angle between the normal of (j) and the direction from (j) to (i)
					//	Hij is the parametric scalar representing the amount of (j) that is visible to (i)
					//	dAj is the differential area (i.e. the area of (i) divided by area of (j))

					// Vector from i to j

					geom::Vector3	ijVect = ec - emitter.origin();
					float		distanceSquared = ijVect.lengthSquared();
					ijVect.normalize();

					float	cosTheta_i = emitter.plane().normal() ^ (ijVect);
					float	cosTheta_j = prim.plane().normal() ^ (-ijVect);
					float	Hij = area_j / totalArea;
					float	dAj = totalArea;

					// Our differential form factor

					float	Fij = (cosTheta_i * cosTheta_j) / (fstl::pi<float>() * distanceSquared) * Hij * dAj;

					// The energy received

					geom::Color3	energy = shootingEnergy * Fij;
					geom::Color3	reflected = energy * originalPrim.reflectanceColor();

					// Keep track of the energy going back into the scene

					receivedEnergy += energy;
					reflectedEnergy += reflected;

					// Store the energy in the element

					se.data()[v*se.width()+u] += reflected * static_cast<float>(areaLightMultiplier());

					// Transfer the energy up to the patch

					if (!directLightOnly())
					{
						unsigned int	pu = u - ominU;
						unsigned int	pv = v - ominV;
						unsigned int	patchIndex = pv/subdivisionV() * originalPrim.uPatches() + pu/subdivisionU();
						RadPatch & patch = originalPrim.patches()[patchIndex];
						patch.energy() += reflected;
					}
				}

				elementCenter += prim.vXFormVector();
			}
		}
	}

	if (directLightOnly()) reflectedEnergy = geom::Color3(0,0,0);

	energyEscapedThisIteration() = shootingEnergy - receivedEnergy;
	energyAbsorbedThisIteration() = shootingEnergy - energyEscapedThisIteration() - reflectedEnergy;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::emitNusselt(RadPatch & emitter, const RadPrimPointerListGrainy & potentialReceivers, RadLMapArray & storedEnergy)
{
	// What's visible from this emitter?

	RadPrimListGrainy	visiblePieces;
	calcVisiblePieces(emitter, potentialReceivers, visiblePieces);

	// Clear out the emitter's energy before we shoot. Theoretically, we never should need to do this, because an emitter
	// should never be able to emit energy onto itself, but it won't hurt to do this for completeness.

	geom::Color3	shootingEnergy = emitter.energy();
	geom::Color3	reflectedEnergy(0,0,0);
	geom::Color3	receivedEnergy(0,0,0);
	emitter.energy() = geom::Color3(0,0,0);

	// Light up the visible pieces
	{
		for (RadPrimListGrainy::node * i = visiblePieces.head(); i; i = i->next())
		{
			RadPrim &	prim = i->data();
			RadPrim &	originalPrim = *i->data().originalPrimitive();
			RadLMap &	se = storedEnergy[originalPrim.textureID()];

			prim.prepareNoPatches();
			unsigned int	fIndex = 0;

			float	totalArea = (prim.uXFormVector() % prim.vXFormVector()).length();

			// We are doing our form factors a bit differently, which allows us to precalculate this :)

			geom::Point3	elementCenter = prim.minXYZ() + prim.uXFormVector() * 0.5 + prim.vXFormVector() * 0.5;

			unsigned int	minU = static_cast<unsigned int>(prim.minUV().u());
			unsigned int	minV = static_cast<unsigned int>(prim.minUV().v());
			unsigned int	maxU = static_cast<unsigned int>(prim.maxUV().u());
			unsigned int	maxV = static_cast<unsigned int>(prim.maxUV().v());

			unsigned int	ominU = static_cast<unsigned int>(originalPrim.minUV().u());
			unsigned int	omaxU = static_cast<unsigned int>(originalPrim.maxUV().u());
			unsigned int	ominV = static_cast<unsigned int>(originalPrim.minUV().v());

			for (unsigned int v = static_cast<unsigned int>(prim.minUV().v()); v <= static_cast<unsigned int>(prim.maxUV().v()); ++v)
			{
				geom::Point3	ec = elementCenter;
				for (unsigned int u = static_cast<unsigned int>(prim.minUV().u()); u <= static_cast<unsigned int>(prim.maxUV().u()); ++u, ++fIndex, ec += prim.uXFormVector())
				{
					// Receiver must have area

					float	area_j = prim.elementAreas()[fIndex];
					if (area_j <= 0) continue;

					// Ignore elements behind the emitter

					if (((ec - emitter.origin()) ^ emitter.plane().normal()) < 0) continue;

					// Ignore same planes

					if ((prim.plane().normal() ^ emitter.plane().normal()) > 0.009f) continue;

					// Get the whole area of this patch from the original polygon

					unsigned int	pu = u - ominU;
					unsigned int	pv = v - ominV;
					unsigned int	elementIndex = pv * (omaxU-ominU+1) + pu;

					// Here we calculate Nusselt's analogy. It is defined as:
					//
					// The form factor of an element (i.e. amount of light the element receives from this patch)
					// is equivalent to the fraction of the unit circle that is formed by taking the projection of
					// the element onto the hemisphere surface and projecting it down onto the circle.
					//
					// Without a diagram, this is hard to understand, so here it is, a bit clearer:
					//
					// We want to know how much energy (from this patch) should be attributed to an element.
					// So you take a unit sphere and center it on the patch. Note that the patch is a planar
					// entity, so we're only concerned with the hemisphere that is on the front-side of the
					// patch. Note also, that since this is a unit hemisphere, the "base" of the hemisphere
					// (where it touches the patch) forms a unit circle.
					//
					// So, Nussalt's analogy is to project a patch onto the surface of the hemisphere. However,
					// doing this is only part of the Equation. This would give elements at sharp angles an
					// equal amount of light as those near the top of the hemisphere (i.e. directly in front of
					// the patch.) This is incorrect; what we need is a non-linear ratio of how much energy each
					// patch gets. So we perform another step -- these elements are then projected orthogonally
					// downward, from the surface of the hemisphere onto its base.
					//
					// Because of this orthogonal projection, we end up with a non-linear scaling of the size
					// of the elements, where those elements near the top of the unit hemisphere will have a larger
					// orthogonal projection than those that were along the sides of the unit hemisphere.
					//
					// We now calculate the area of those final projected elements. If you add up all the areas,
					// you will find that they total the area of a single unit circle, because they were all
					// projected onto a unit circle. So we divide these areas by PI (i.e. the area of a unit circle)
					// and we have the ratio for our form factor.
					//
					// If this is confusing, just peek at a diagram of Nusselt's Analogy, and you'll see it's actually
					// quite simple.

					float	nusseltEnergy = 0;
					{
						geom::Point3	newec = ec - prim.uXFormVector() * 0.5 - prim.vXFormVector() * 0.5;

						geom::Vector3	v0 = newec - emitter.origin();
						v0.normalize();
						geom::Vector3	v1 = newec + prim.uXFormVector() - emitter.origin();
						v1.normalize();
						geom::Vector3	v2 = newec + prim.uXFormVector() + prim.vXFormVector() - emitter.origin();
						v2.normalize();
						geom::Vector3	v3 = newec + prim.vXFormVector() - emitter.origin();
						v3.normalize();

						v0 = emitter.plane().closest(v0 + emitter.origin());
						v1 = emitter.plane().closest(v1 + emitter.origin());
						v2 = emitter.plane().closest(v2 + emitter.origin());
						v3 = emitter.plane().closest(v3 + emitter.origin());

						RadPrim orthoProjection;
						orthoProjection.xyz() += v0;
						orthoProjection.xyz() += v1;
						orthoProjection.xyz() += v2;
						orthoProjection.xyz() += v3;
						nusseltEnergy = orthoProjection.calcArea() / fstl::pi<float>();

						// We now account for the fraction of this patch as it clips to the primitive

						nusseltEnergy *= area_j / totalArea;
					}

					// The energy received

					geom::Color3	energy = shootingEnergy * nusseltEnergy;
					geom::Color3	reflected = energy * originalPrim.reflectanceColor();

					// Keep track of the energy going back into the scene

					receivedEnergy += energy;
					reflectedEnergy += reflected;

					// Store the energy in the element

					se.data()[v*se.width()+u] += reflected * static_cast<float>(areaLightMultiplier());

					// Transfer the energy up to the patch

					if (!directLightOnly())
					{
						unsigned int	pu = u - ominU;
						unsigned int	pv = v - ominV;
						unsigned int	patchIndex = pv/subdivisionV() * originalPrim.uPatches() + pu/subdivisionU();
						RadPatch & patch = originalPrim.patches()[patchIndex];
						patch.energy() += reflected;
					}
				}

				elementCenter += prim.vXFormVector();
			}
		}
	}

	if (directLightOnly()) reflectedEnergy = geom::Color3(0,0,0);

	energyEscapedThisIteration() = shootingEnergy - receivedEnergy;
	energyAbsorbedThisIteration() = shootingEnergy - energyEscapedThisIteration() - reflectedEnergy;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::emitPointLight(RadPatch & emitter, const RadPrimPointerListGrainy & potentialReceivers, RadLMapArray & storedEnergy)
{
	// What's visible from this emitter?

	RadPrimListGrainy	visiblePieces;
	calcVisiblePieces(emitter, potentialReceivers, visiblePieces);

	// We're emitting from a point light source. This isn't necessarily how Radiosity works, so we need to be
	// able to translate from "energy transmitted from a point light source" to "energy transmitted from an area
	// light source" -- and here it is....

	float	pointToAreaScaleFactor = pointLightMultiplier() / areaLightMultiplier();

	// Clear out the emitter's energy before we shoot. Theoretically, we never should need to do this, because an emitter
	// should never be able to emit energy onto itself, but it won't hurt to do this for completeness.

	geom::Color3	shootingEnergy = emitter.energy();
	geom::Color3	reflectedEnergy(0,0,0);
	geom::Color3	receivedEnergy(0,0,0);
	emitter.energy() = geom::Color3(0,0,0);

	// Light up the visible pieces
	{
		for (RadPrimListGrainy::node * i = visiblePieces.head(); i; i = i->next())
		{
			RadPrim &	prim = i->data();
			RadPrim &	originalPrim = *i->data().originalPrimitive();
			RadLMap &	se = storedEnergy[originalPrim.textureID()];

			prim.prepareNoPatches();
			unsigned int	fIndex = 0;

			float	totalArea = (prim.uXFormVector() % prim.vXFormVector()).length();

			geom::Point3	elementCenter = prim.minXYZ() + prim.uXFormVector() * 0.5 + prim.vXFormVector() * 0.5;

			// PDNDEBUG -- could this code be responsible for the reason why we get strange edges at the lightmap-clipped limits? Not using floor()?

			unsigned int	minU = static_cast<unsigned int>(prim.minUV().u());
			unsigned int	minV = static_cast<unsigned int>(prim.minUV().v());
			unsigned int	maxU = static_cast<unsigned int>(prim.maxUV().u());
			unsigned int	maxV = static_cast<unsigned int>(prim.maxUV().v());

			unsigned int	ominU = static_cast<unsigned int>(originalPrim.minUV().u());
			unsigned int	omaxU = static_cast<unsigned int>(originalPrim.maxUV().u());
			unsigned int	ominV = static_cast<unsigned int>(originalPrim.minUV().v());

			for (unsigned int v = minV; v <= maxV; ++v)
			{
				geom::Point3	ec = elementCenter;
				for (unsigned int u = minU; u <= maxU; ++u, ++fIndex, ec += prim.uXFormVector())
				{
					// Receiver must have area

					float	area_j = prim.elementAreas()[fIndex];
					if (area_j <= 0) continue;

					// Get the whole area of this patch from the original polygon

					unsigned int	pu = u - ominU;
					unsigned int	pv = v - ominV;
					unsigned int	elementIndex = pv * (omaxU-ominU+1) + pu;

					// Here we calculate a hacked radiosity equation for a point light. It's just this:
					//
					//    1 / (distance_to_element * distance_to_element)

					// Vector from j to i

#if 0 // Quake-like lighting
					geom::Vector3	jiVect = emitter.origin() - ec;

					float		distanceSquared = jiVect.lengthSquared() / (8*8); // these 8's are specific to Jason's OCT files

					jiVect.normalize();
					float	Hij = area_j / totalArea;
					float	Fij = (1.0f / distanceSquared) * (jiVect^prim.plane().normal()) * Hij;
#else
					geom::Vector3	jiVect = emitter.origin() - ec;
					float		distanceSquared = jiVect.lengthSquared();
					jiVect.normalize();

					float	cosTheta_j = prim.plane().normal() ^ jiVect;
					float	Hij = area_j / totalArea;
					float	dAj = totalArea;

					// Our differential form factor

					float	Fij = cosTheta_j / (fstl::pi<float>() * distanceSquared) * Hij * dAj;
#endif
					// The energy received

					geom::Color3	energy = shootingEnergy * Fij;
					geom::Color3	reflected = energy * originalPrim.reflectanceColor();

					// Keep track of the energy going back into the scene

					receivedEnergy += energy;
                                        reflectedEnergy += reflected;

					// Store the energy in the element

					se.data()[v*se.width()+u] += reflected * pointLightMultiplier();

					// Transfer the energy up to the patch

					if (!directLightOnly())
					{
						unsigned int	patchIndex = pv/subdivisionV() * originalPrim.uPatches() + pu/subdivisionU();
						RadPatch & patch = originalPrim.patches()[patchIndex];
						patch.energy() += reflected * pointToAreaScaleFactor;
					}
				}

				elementCenter += prim.vXFormVector();
			}
		}
	}

	// Notice how we do not subtract the reflected energy from this value like we do for the area light sources...
	//
	// We don't do this for point lights because it's pretty much uselss... they don't act like normal lights, and have a
	// different scale for lighting alltogether. So, we treat them separately, and give them a separate illumination pass.
	//
	// Therefore, we don't need to (or want to) track the energy reflected back into the scene in the statistics.

	energyAbsorbedThisIteration() = shootingEnergy * pointToAreaScaleFactor;
}

// ---------------------------------------------------------------------------------------------------------------------------------

RadPatch *	RadGen::findBrightestEmitter()
{
	RadPatch *	brightestPatch = static_cast<RadPatch *>(0);
	float		brightestEmission = 0.0f;

	// Light sources come first...

	if (lightsToProcess())
	{
		for(RadPatchList::node *i = geometry().lights().head(); i; i = i->next())
		{
			RadPatch &	p = i->data();

			// Ignore polygons that don't illuminate

			if (p.energy() == geom::Color3(0,0,0)) continue;

			// Is this the brightest (or only one) so far?

			float	thisEmission = p.energy().r() + p.energy().g() + p.energy().b();

			if (thisEmission > brightestEmission)
			{
				brightestEmission = thisEmission;
				brightestPatch = &p;
			}
		}
	}

	// If there is a light, use it first

	if (brightestPatch) return brightestPatch;

	// If we get here, we have no more lights to process

	lightsToProcess() = false;

	for(RadPrimList::node * i = geometry().polys().head(); i; i = i->next())
	{
		RadPrim &	prim = i->data();

		for(unsigned int j = 0; j < prim.patches().size(); ++j)
		{
			RadPatch &	p = prim.patches()[j];

			// Ignore polygons that don't illuminate

			if (p.energy() == geom::Color3(0,0,0)) continue;

			// Ignore unused patches

			if (!p.area()) continue;

			// Is this the brightest (or only one) so far?

			float	thisEmission = p.energy().r() + p.energy().g() + p.energy().b();

			if (thisEmission > brightestEmission)
			{
				brightestEmission = thisEmission;
				brightestPatch = &p;
			}
		}
	}

	return brightestPatch;
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	RadGen::processEnergy(SOctree & node)
{
	// Bounce some light around

	bool		processingLights = lightsToProcess();
	bool		atMaxSubdivision = false;
	unsigned int	iterationsToPerform = maxIterationsCount();
	if (!maxIterations())	iterationsToPerform = 0xffffffff;

	while(iterationsToPerform)
	{
		// Find the brightest emitter

		RadPatch *	emitter = findBrightestEmitter();
		if (!emitter) break;

		energyThisPass() = emitter->energy().r() + emitter->energy().g() + emitter->energy().b();

		// What are we doing?

		if (lightsToProcess())
		{
			// DJ_TEMP : progress()->setCurrentStatus("Distributing energy from light sources");

			energyThisPass() *= pointLightMultiplier() / areaLightMultiplier();
		}
		else
		{
			if (processingLights)
			{
				processingLights = false;
				totalAbsorbedEnergy() = geom::Color3(0,0,0);
				totalEscapedEnergy() = geom::Color3(0,0,0);
				initialEnergy() = calcTotalEnergy();
			}

			// DJ_TEMP : progress()->setCurrentStatus("Distributing reflected energy");
		}

		// Can we merge patches?

		unsigned int	newSubdivisionU = subdivisionU() * 2;
		unsigned int	newSubdivisionV = subdivisionV() * 2;

		if (!atMaxSubdivision && adaptivePatchSubdivision() && newSubdivisionU <= adaptiveMaxSubdivisionU() && newSubdivisionV <= adaptiveMaxSubdivisionV())
		{
			// Have we reached the adaptive subdivision threshold?

			geom::Color3	temp = calcRemainingEnergy();
			float	remaining = temp.r() + temp.g() + temp.b();

			if (energyThisPass() * areaLightMultiplier() < adaptiveThreshold())
			{
				// Merge patches

				bool	merged = false;

				for(RadPrimList::node * i = geometry().polys().head(); i; i = i->next())
				{
					RadPrim &	prim = i->data();

					if (prim.mergePatches2x2()) merged = true;
				}

				// Was anything merged?

				if (merged)
				{
					// Our new subdivision levels

	 				subdivisionU() = newSubdivisionU;
	 				subdivisionV() = newSubdivisionV;

					// Update our elements/patch count

					countPatchesAndElements();

					// Back to the top and try again...

					continue;
				}

				// Nothing could be merged, so stop trying

				else
				{
					atMaxSubdivision = true;
				}
			}
		}

		// Update the user

		if (!updateStats()) return false;

		// Find potential receivers

		RadPrimPointerListGrainy	potentialReceivers;
		node.bsp().getOrderedList(emitter->origin(), potentialReceivers);

		// Emit the energy
		{
			energyAbsorbedThisIteration() = geom::Color3(0,0,0);

			if (emitter->isPointLight())	emitPointLight(*emitter, potentialReceivers, geometry().lightmaps());
			else if (useNusselt())		emitNusselt(*emitter, potentialReceivers, geometry().lightmaps());
			else				emit(*emitter, potentialReceivers, geometry().lightmaps());

			// Track the total absorbed and escaped energy

                        totalAbsorbedEnergy() += energyAbsorbedThisIteration();
                        totalEscapedEnergy() += energyEscapedThisIteration();
		}

		// Track iterations

		iterationsToPerform--;
		iterationsProcessed()++;
	}

	return true;
}

// ---------------------------------------------------------------------------------------------------------------------------------

geom::Color3	RadGen::calcTotalEnergy()
{
	geom::Color3	remain(0,0,0);

	// Lights
	{
		float	pointToAreaScaleFactor = pointLightMultiplier() / areaLightMultiplier();

		for(RadPatchList::node * i = geometry().lights().head(); i; i = i->next())
		{
			remain += i->data().energy() * pointToAreaScaleFactor;
		}
	}

	// Patches
	{
		for(RadPrimList::node * i = geometry().polys().head(); i; i = i->next())
		{
			RadPrim &	prim = i->data();

			for(unsigned int j = 0; j < prim.patches().size(); ++j)
			{
				remain += prim.patches()[j].energy();
			}
		}
	}

	return remain;
}

// ---------------------------------------------------------------------------------------------------------------------------------

geom::Color3	RadGen::calcRemainingEnergy()
{
	return initialEnergy() - totalAbsorbedEnergy() - totalEscapedEnergy();
}

// ---------------------------------------------------------------------------------------------------------------------------------

bool	RadGen::updateStats(const bool checkConvergence)
{
	const	float	displayMultiplier = static_cast<float>(areaLightMultiplier());

	geom::Color3	ie = initialEnergy();
	geom::Color3	ae = totalAbsorbedEnergy();
	geom::Color3	ee = totalEscapedEnergy();

	float	initial = ie.r() + ie.g() + ie.b();
	float	absorbed = ae.r() + ae.g() + ae.b();
	float	escaped = ee.r() + ee.g() + ee.b();
	float	remaining = initial - absorbed - escaped;

	float	percentThisPass = energyThisPass() / remaining * 100;
	float	percentEnergy = absorbed / (initial - escaped) * 100;
	// DJ_TEMP : prog.setCurrentPercent(percentEnergy);

    // DJ_TEMP : 
    /*
	prog.setLabel1("Iterations:");
	if (maxIterations())
	{
		prog.setText1(fstl::string(iterationsProcessed()) + " of " + fstl::string(maxIterationsCount()));
	}
	else
	{
		prog.setText1(fstl::string(iterationsProcessed()));
	}

	prog.setLabel2("Initial (abs/esc) Energy:");
	char	disp[MAX_PATH];
	if (lightsToProcess())
	{
		sprintf(disp, "[processing lights]");
	}
	else
	{
		sprintf(disp, "%u (%.2f%% / %.2f%%)", static_cast<unsigned int>(initial * displayMultiplier), absorbed/initial*100, escaped/initial*100);
	}
	prog.setText2(disp);

	prog.setLabel3("Energy remaining:");
	prog.setText3(fstl::string(static_cast<unsigned int>(remaining * displayMultiplier)));

	prog.setLabel4("Convergence target:");

	if (checkConvergence && !directLightOnly())
	{
		sprintf(disp, "%u (%.5f%%)", convergence(), static_cast<float>(convergence()) / (energyThisPass() * displayMultiplier) * 100);
		prog.setText4(disp);
	}
	else
	{
		if (directLightOnly())	prog.setText4("N/A (direct light only)");
		else			prog.setText4("N/A");
	}

	prog.setLabel5("Energy shot this pass:");
	sprintf(disp, "%.2f", energyThisPass() * displayMultiplier);
	prog.setText5(disp);

	prog.setLabel6("Form factor calculation method:");
	if (lightsToProcess())
	{
		prog.setText6("Point light estimation");
	}
	else
	{
		if (useNusselt())	prog.setText6("Nusselt's Analogy");
		else			prog.setText6("Standard estimation");
	}

	prog.setLabel7("Input file:");
	fstl::string	ifn = inputFilename();
	int	idx = ifn.rfind("\\");
	if (idx != -1) ifn = ifn.substring(idx+1);
	prog.setText7(ifn);

	if (adaptivePatchSubdivision())
	{
		prog.setLabel8("Adaptive subdivision threshold:");
		sprintf(disp, "%u (%.3f%% there)", adaptiveThreshold(), adaptiveThreshold() / (energyThisPass() * displayMultiplier) * 100);
		prog.setText8(disp);
	}
	else
	{
		prog.setLabel8("");
		prog.setText8("");
	}

	prog.setLabel9("Total patches:");
	prog.setText9(fstl::string(totalPatches()) + " (cur: " + fstl::string(subdivisionU()) + "x" + fstl::string(subdivisionV()) + ", max: " + fstl::string(adaptiveMaxSubdivisionU()) + "x" + fstl::string(adaptiveMaxSubdivisionV()) + ")");

	prog.setLabel10("Total elements:");
	prog.setText10(fstl::string(totalElements()));
    */

	// If they've reached convergence, tell them to stop
	//
	// Note that we don't check for convergence when told, and when there are lights to process, because we want at least
	// all light sources processed, as a minimum.

	if (!lightsToProcess() && checkConvergence && (energyThisPass() * displayMultiplier) <= convergence()) return false;

	// Or tell them to stop when the user requests it...

	return true;// DJ_TEMP : !prog.stopRequested();
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::distributeInitialEnergy()
{
	// DJ_TEMP : progress()->setCurrentStatus("Slicing up geometry into patches/elements");

	unsigned int	idx = 0;
	for (RadPrimList::node *i = geometry().polys().head(); i; i = i->next(), ++idx)
	{
        // DJ_TEMP : 
        /*
		if (!(idx&0xf))
		{
			progress()->setCurrentPercent(static_cast<float>(idx) / static_cast<float>(geometry().polys().size()) * 100.0f);
			if (progress()->stopRequested()) throw "";
		}
        */

		RadPrim &	prim = i->data();
		prim.originalPrimitive() = &prim;
		prim.prepare(subdivisionU(), subdivisionV());

		if (prim.illuminationColor() != geom::Color3(0,0,0))
		{
			int	minU, maxU, minV, maxV;
			prim.calcIntegerUVExtents(minU, maxU, minV, maxV);

			RadLMap	&	lm = geometry().lightmaps()[prim.textureID()];

			float	totalArea = (prim.uXFormVector() % prim.vXFormVector()).length();
			float *	areas = &prim.elementAreas()[0];
			for (unsigned int v = minV; v <= static_cast<unsigned int>(maxV); ++v)
			{
				for (unsigned int u = minU; u <= static_cast<unsigned int>(maxU); ++u, ++areas)
				{
					if (*areas)
					{
						float	partialArea = *areas / totalArea;
						lm.data()[v*lm.width()+u] = prim.illuminationColor() * static_cast<float>(areaLightMultiplier()) * partialArea;
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::countPatchesAndElements()
{
	// DJ_TEMP : progress()->setCurrentStatus("Counting patches and elements");

	totalPatches() = 0;
	totalElements() = 0;

	unsigned int	idx = 0;
	for (RadPrimList::node *i = geometry().polys().head(); i; i = i->next(), ++idx)
	{
        // DJ_TEMP : 
        /*
		if (!(idx&0xf))
		{
			progress()->setCurrentPercent(static_cast<float>(idx) / static_cast<float>(geometry().polys().size()) * 100.0f);
			if (progress()->stopRequested()) throw "";
		}
        */

		RadPrim &	prim = i->data();

		// Count patches

		for (unsigned int j = 0; j < prim.patches().size(); ++j)
		{
			if (prim.patches()[j].area()) totalPatches()++;
		}

		for (unsigned int k = 0; k < prim.elementAreas().size(); ++k)
		{
			if (prim.elementAreas()[k]) totalElements()++;
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

geom::Color3	RadGen::calcAmbientTerm()
{
	// DJ_TEMP : progress()->setCurrentStatus("Calculating the ambient term");

	// We'll need the total area and total average reflectivity

	float		totalArea = 0;
	geom::Color3	pAve(0,0,0);
	{
		for (RadPrimList::node *i = geometry().polys().head(); i; i = i->next())
		{
			RadPrim &	p = i->data();
			float	area = p.calcArea();
			totalArea += area;
			pAve += p.reflectanceColor() * area;
		}
	}

	// Calculate the ambient term

	pAve /= totalArea;
	geom::Color3	totalUnshotEnergy = (initialEnergy() - totalAbsorbedEnergy() - totalEscapedEnergy());
	geom::Color3	estimatedAmbientTerm = pAve * totalUnshotEnergy / totalArea;

	unsigned int	idx = 0;
	for (RadPrimList::node *i = geometry().polys().head(); i; i = i->next(), ++idx)
	{
        // DJ_TEMP : 
        /*
		if (!(idx&0xf))
		{
			progress()->setCurrentPercent(static_cast<float>(idx) / static_cast<float>(geometry().polys().size()) * 100.0f);
			if (progress()->stopRequested()) throw "";
		}
        */

		RadPrim &	p = i->data();
		RadLMap &	lm = geometry().lightmaps()[p.textureID()];

		int	minU, maxU, minV, maxV;
		p.calcIntegerUVExtents(minU, maxU, minV, maxV);

		float	totalArea = (p.uXFormVector() % p.vXFormVector()).length();

		// Remove softened edges around polygons
		{
			int	fIndex = 0;
			for (unsigned int v = minV; v <= static_cast<unsigned int>(maxV); ++v)
			{
				for (unsigned int u = minU; u <= static_cast<unsigned int>(maxU); ++u, ++fIndex)
				{
					// Area

					float	eArea = p.elementAreas()[fIndex];

					// Skip unused texels

					if (eArea <= 0) continue;

					// Add the ambient term

					lm.data()[v*lm.width()+u] += estimatedAmbientTerm * p.reflectanceColor() * eArea * static_cast<float>(areaLightMultiplier());
				}
			}
		}
	}

	return estimatedAmbientTerm;
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::expandEdges()
{
	// DJ_TEMP : progress()->setCurrentStatus("Correcting edges in the lightmaps");

	// Duplicate the lightmaps for mask purposes
// PDNDEBUG -- maskMaps is a bad name.. and using lightmaps is wasteful... could be just an array of float arrays...
	RadLMapArray		maskMaps = geometry().lightmaps();

	// Clear out the maskmaps
	{
		geom::Color3	blank(0,0,0);

		for (unsigned int i = 0; i < maskMaps.size(); ++i)
		{
			RadLMap &	mm = maskMaps[i];
			unsigned int	size = mm.width() * mm.height();
			geom::Color3 *	mPtr = &mm.data()[0];
			for (unsigned int i = 0; i < size; ++i, ++mPtr)
			{
				*mPtr = blank;
			}
		}
	}

// PDNDEBUG - hack
float	totalArea = 0;

	// Fill the mask with element areas (i.e. this is essentially the antialiasing information)
	{
		unsigned int	idx = 0;
		for (RadPrimList::node *i = geometry().polys().head(); i; i = i->next(), ++idx)
		{
            // DJ_TEMP : 
            /*
			if (!(idx&0xf))
			{
				progress()->setCurrentPercent(static_cast<float>(idx) / static_cast<float>(geometry().polys().size()) * 50.0f);
				if (progress()->stopRequested()) throw "";
			}
            */

			RadPrim &	p = i->data();
			RadLMap &	mm = maskMaps[p.textureID()];

// PDNDEBUG - hack -- can we replace all calculations of "total area" with a global value, now that we're creating our own lightmaps?
totalArea = (p.uXFormVector() % p.vXFormVector()).length();

			int	minU, maxU, minV, maxV;
			p.calcIntegerUVExtents(minU, maxU, minV, maxV);

			// We'll go through and place the element areas where polygons occupy the lightmaps
			{
				unsigned int	fIndex = 0;
				for (unsigned int v = minV; v <= static_cast<unsigned int>(maxV); ++v)
				{
					unsigned int	vIndex = v * mm.width();
					for (unsigned int u = minU; u <= static_cast<unsigned int>(maxU); ++u, ++fIndex)
					{
						// If this texel is not occupied, skip it

						if (!p.elementAreas()[fIndex]) continue;

						// ADD the texel area into the red component of the lightmap for this texel
						//
						// This allows us to know the total usage of a texel from a series of possibly
						// connected polygons

						mm.data()[vIndex + u].r() += p.elementAreas()[fIndex];
					}
				}
			}
		}
	}

	// Un-antialias the lightmaps
	{
		for (unsigned int i = 0; i < maskMaps.size(); ++i)
		{
			RadLMap &	mm = maskMaps[i];
			RadLMap &	lm = geometry().lightmaps()[i];
			geom::Color3 *	mPtr = &mm.data()[0];
			geom::Color3 *	cPtr = &lm.data()[0];

			unsigned int	size = mm.width() * mm.height();
			for (unsigned int i = 0; i < size; ++i, ++mPtr, ++cPtr)
			{
				// Skip unoccupied or wholly occupied pixels

				if (mPtr->r() == 0 || mPtr->r() >= totalArea) continue;

				// Adjust lightmap texel -- this is effectively un-antialiasing the texel

				*cPtr *= totalArea / mPtr->r();
			}
		}
	}

	// Do the filtering...
	{
		for (unsigned int i = 0; i < geometry().lightmaps().size(); ++i)
		{
            // DJ_TEMP : 
            /*
			if (!(i&0xf))
			{
				progress()->setCurrentPercent(static_cast<float>(i) / static_cast<float>(geometry().polys().size()) * 50.0f + 50.0f);
				if (progress()->stopRequested()) throw "";
			}
            */

			// Lightmap & mask

			RadLMap &	lm = geometry().lightmaps()[i];
			RadLMap &	mm = maskMaps[i];
			unsigned int	lw = lm.width();
			unsigned int	lh = lm.height();

			// Here we do our pixel extension... it works like this:
			//
			// Each pixel that does _not_ contain any mask area, will be the averge of its neighbors that _do_ have
			// a mask area (if any).
			{
				for (unsigned int v = 0; v < lh; ++v)
				{
					// Current row for the [M]ask and [C]olor maps (for convenience)...

					geom::Color3 *	cmRow = &mm.data()[v*lw];
					geom::Color3 *	ccRow = &lm.data()[v*lw];

					// Top row for the [M]ask and [C]olor maps (for convenience)...

					geom::Color3 *	tmRow = static_cast<geom::Color3 *>(0);
					geom::Color3 *	tcRow = static_cast<geom::Color3 *>(0);
					if (v > 0)
					{
						tmRow = cmRow - lw;
						tcRow = ccRow - lw;
					}

					// Bottom row for the [M]ask and [C]olor maps (for convenience)...

					geom::Color3 *	bmRow = static_cast<geom::Color3 *>(0);
					geom::Color3 *	bcRow = static_cast<geom::Color3 *>(0);
					if (v < lh-1)
					{
						bmRow = cmRow + lw;
						bcRow = ccRow + lw;
					}

					for (unsigned int u = 0; u < lw; ++u)
					{
						// We're only averaging non-masked (i.e. un-occupied) pixels

						if (cmRow[u].r()) continue;

						// Average each of the 8 neighboring pixels (that have mask colors)

						unsigned int	count = 0;
						geom::Color3	totalColor(0,0,0);

						// Top row

						if (tmRow)
						{
							if (u >    0 && tmRow[u-1].r()) totalColor += tcRow[u-1], ++count;
							if (            tmRow[u  ].r()) totalColor += tcRow[u  ], ++count;
							if (u < lw-1 && tmRow[u+1].r()) totalColor += tcRow[u+1], ++count;
						}

						// Current row

						{
							if (u >    0 && cmRow[u-1].r()) totalColor += ccRow[u-1], ++count;
							if (u < lw-1 && cmRow[u+1].r()) totalColor += ccRow[u+1], ++count;
						}

						// Bottom row

						if (bmRow)
						{
							if (u >    0 && bmRow[u-1].r()) totalColor += bcRow[u-1], ++count;
							if (            bmRow[u  ].r()) totalColor += bcRow[u  ], ++count;
							if (u < lw-1 && bmRow[u+1].r()) totalColor += bcRow[u+1], ++count;
						}

						// The average

						if (count)	ccRow[u] = totalColor / static_cast<float>(count);
						else		ccRow[u] = geom::Color3(0,0,0);
					}
				}
			}
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::addAmbient()
{
	if (ambient() == geom::Color3(0,0,0)) return;

	// DJ_TEMP : progress()->setCurrentStatus("Adding post-processing ambient light");

	for (unsigned int i = 0; i < geometry().lightmaps().size(); ++i)
	{
        // DJ_TEMP : 
        /*
		if (!(i&0xf))
		{
			progress()->setCurrentPercent(static_cast<float>(i) / static_cast<float>(geometry().lightmaps().size()) * 100.0f);
			if (progress()->stopRequested()) throw "";
		}
        */

		geometry().lightmaps()[i].addAmbient(ambient());
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::doGammaCorrection()
{
	if (gamma() == 1.0f) return;

	// DJ_TEMP : progress()->setCurrentStatus("Gamma correction");

	for (unsigned int i = 0; i < geometry().lightmaps().size(); ++i)
	{
        // DJ_TEMP : 
        /*
		if (!(i&0xf))
		{
			progress()->setCurrentPercent(static_cast<float>(i) / static_cast<float>(geometry().lightmaps().size()) * 100.0f);
			if (progress()->stopRequested()) throw "";
		}
        */

		geometry().lightmaps()[i].applyGamma(gamma());
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::doClamping()
{
	if (clamping() == ClampSaturate)
	{
		// DJ_TEMP : progress()->setCurrentStatus("Clamping with saturation");

		for (unsigned int i = 0; i < geometry().lightmaps().size(); ++i)
		{
            // DJ_TEMP : 
            /*
			if (!(i&0xf))
			{
				progress()->setCurrentPercent(static_cast<float>(i) / static_cast<float>(geometry().lightmaps().size()) * 100.0f);
				if (progress()->stopRequested()) throw "";
			}
            */

			geometry().lightmaps()[i].clampSaturate();
		}
	}
	else if (clamping() == ClampRetain)
	{
		// DJ_TEMP : progress()->setCurrentStatus("Clamping with color ratio");

		for (unsigned int i = 0; i < geometry().lightmaps().size(); ++i)
		{
            // DJ_TEMP : 
            /*
			if (!(i&0xf))
			{
				progress()->setCurrentPercent(static_cast<float>(i) / static_cast<float>(geometry().lightmaps().size()) * 100.0f);
				if (progress()->stopRequested()) throw "";
			}
            */

			geometry().lightmaps()[i].clampRetainColorRatio();
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::go()
{
	try
	{
		// Load up the geometry
        // DJ_TEMP : TODO
        if (!geometry().ReadGeomety(defaultReflectivity()))
        {
            // DJ_TEMP : if (!progress()->stopRequested())
            {
                throw "Unable to load the geometry";
            }
        }

		// Generate lightmaping coordinates and lightmaps for these polygons
		{
			LMapGen	lmg;
			lmg.lightmapWidth() = lightmapWidth();
			lmg.lightmapHeight() = lightmapHeight();
			lmg.uTexelsPerUnit() = uTexelsPerUnit();
			lmg.vTexelsPerUnit() = vTexelsPerUnit();

			if (!lmg.generate(geometry().polys(), geometry().lightmaps()))
			{
				throw "Unable to generate lightmap coordinates & lightmaps";
			}
		}

		// Initial energy goes into the lightmaps to color the lights of their own color

		distributeInitialEnergy();

		// Count up the total patches & elements

		countPatchesAndElements();

		// Build the octree

		SOctree	octree;
		SOctree::sBuildInfo bi;
		{
			bi.thresholdLimiter = octreeThreshold();
			bi.minRadiusLimiter = octreeMinRadius();
			bi.maxDepthLimiter = octreeMaxDepth();
			bi.bspQuantizeResolution = bspGaussianResolution();
			bi.bspLeastDepthErrorBoundsPercent = bspMinSplitRange();
			bi.patchSubdivisionU = subdivisionU();
			bi.patchSubdivisionV = subdivisionV();

			if (!octree.build(geometry(), bi))
			{
				// Error if they didn't ask to stop

				// DJ_TEMP : if (!progress()->stopRequested()) throw "Unable to build the Octree";

				// If they _did_ ask to stop, bail without an error

				// else throw "";

                // DJ_TEMP : Added by me
                throw "Unable to build the Octree";
			}
		}

		// Initial stats

		totalAbsorbedEnergy() = geom::Color3(0,0,0);
		totalEscapedEnergy() = geom::Color3(0,0,0);
		iterationsProcessed() = 0;
		initialEnergy() = calcTotalEnergy();
		energyThisPass() = 0;
		updateStats();
		lightsToProcess() = geometry().lights().size() != 0;

		// Run the ratiosity process

		octree.traverse(_processEnergy, static_cast<void *>(this));

		// Check for a request to bail...

        // DJ_TEMP : 
        /*
		if (progress()->stopRequested())
		{
			if (AfxMessageBox("You have stopped the radiosity process.\n\nSave the data you have generted so far?", MB_YESNO) == IDYES)
			{
				progress()->resetStopped();
			}
			else
			{
				throw "";
			}
		}
        */

		// Ambient term estimation for unshot light

		geom::Color3	estimatedAmbientTerm(0,0,0);
		if (ambientTerm()) estimatedAmbientTerm = calcAmbientTerm();

		// Correct polygon edges within the lightmap & add the ambient term

		expandEdges();

		// Ambient light (yech!)

		addAmbient();

		// Gamma correction

		doGammaCorrection();

		// Clamping/Saturation

		doClamping();

		// Output the OCT file

        // DJ_TEMP : TODO
        /*
		if (writeOctFile())
		{
			progress()->setCurrentStatus("Writing Oct file");
			geometry().writeOCT(writeOctFilename(), *progress());
		}
        */

		// Write the raw data?

        // DJ_TEMP : TODO
        /*
		if (writeRawLightmaps())
		{
			progress()->setCurrentStatus("Writing lightmaps");

			for (unsigned int i = 0; i < geometry().lightmaps().size(); ++i)
			{
				if (!(i&0xf))
				{
					progress()->setCurrentPercent(static_cast<float>(i) / static_cast<float>(geometry().lightmaps().size()) * 100.0f);
					if (progress()->stopRequested()) throw "";
				}

				// Write the lightmap

				geometry().lightmaps()[i].writeRaw(writeRawLightmapsFolder());
			}
		}
        */

		// DONE!
        // DJ_TEMP
	}

    catch(const char *err)
	{
		if (err && *err) AfxMessageBox(err);
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------

void	RadGen::readDefaultParms()
{
    // DJ_TEMP : TODO
    /*
	// Desktop

	char	desktopPath[MAX_PATH];
	memset(desktopPath, 0, sizeof(desktopPath));
	SHGetSpecialFolderPath(NULL, desktopPath, CSIDL_DESKTOPDIRECTORY, 0);
	fstl::string	defaultOctOutput = fstl::string(desktopPath) + "\\output.oct";

	// Lightmap parms

	lightmapWidth() = theApp.GetProfileInt("Options", "lightmapWidth", 128);
	lightmapHeight() = theApp.GetProfileInt("Options", "lightmapHeight", 128);
	uTexelsPerUnit() = 1 / static_cast<float>(atof(theApp.GetProfileString("Options", "uTexelsPerUnitInverse", "16")));
	vTexelsPerUnit() = 1 / static_cast<float>(atof(theApp.GetProfileString("Options", "vTexelsPerUnitInverse", "16")));

	// Octree parameters

	octreeThreshold() = theApp.GetProfileInt("Options", "OctreePolysPerNode", 50000);
	octreeMaxDepth() = theApp.GetProfileInt("Options", "OctreeMaxDepth", 50);
	octreeMinRadius() = static_cast<float>(atof(theApp.GetProfileString("Options", "OctreeMinRadius", "5.0")));

	// Reflectivity

	float	rr = static_cast<float>(atof(theApp.GetProfileString("Options", "rReflectivity", "0.78")));
	float	rg = static_cast<float>(atof(theApp.GetProfileString("Options", "rReflectivity", "0.78")));
	float	rb = static_cast<float>(atof(theApp.GetProfileString("Options", "rReflectivity", "0.78")));
	defaultReflectivity() = geom::Color3(rr, rg, rb);

	// BSP parameters

	bspMinSplitRange() = static_cast<float>(atof(theApp.GetProfileString("Options", "BSPMinSplitRange", "5.0")));
	bspGaussianResolution() = theApp.GetProfileInt("Options", "BSPGaussianResolution", 8);

	// Input/output parameters

	writeRawLightmapsFolder() = fstl::string(theApp.GetProfileString("Options", "outputRawDirectory", desktopPath));
	writeRawLightmaps() = theApp.GetProfileInt("Options", "outputRawDirectoryFlag", 0) ? true:false;
	writeOctFile() = theApp.GetProfileInt("Options", "outputOctFileFlag", 1) ? true:false;
	writeOctFilename() = fstl::string(theApp.GetProfileString("Options", "outputOctFilename", defaultOctOutput.asArray()));
	inputFilename() = fstl::string(theApp.GetProfileString("Options", "inputFilename", ""));

	// General parms

	convergence() = theApp.GetProfileInt("Options", "convergence", 10);
	maxIterations() = theApp.GetProfileInt("Options", "enableMaxIterations", 0) ? true:false;
	maxIterationsCount() = theApp.GetProfileInt("Options", "maxIterationsCount", 4);
	areaLightMultiplier() = theApp.GetProfileInt("Options", "areaLightMultiplier", 1000000);
	pointLightMultiplier() = static_cast<float>(atof(theApp.GetProfileString("Options", "pointLightMultiplier", "0.6")));
	subdivisionU() = theApp.GetProfileInt("Options", "subdivisionU", 4);
	subdivisionV() = theApp.GetProfileInt("Options", "subdivisionV", 4);
	ambientTerm() = theApp.GetProfileInt("Options", "ambientTerm", 1) ? true:false;
	useNusselt() = theApp.GetProfileInt("Options", "useNusselt", 0) ? true:false;
	directLightOnly() = theApp.GetProfileInt("Options", "directLightOnly", 0) ? true:false;
	adaptiveMaxSubdivisionU() = theApp.GetProfileInt("Options", "adaptiveMaxSubdivisionU", 256);
	adaptiveMaxSubdivisionV() = theApp.GetProfileInt("Options", "adaptiveMaxSubdivisionV", 256);
	adaptivePatchSubdivision() = theApp.GetProfileInt("Options", "adaptivePatchSubdivision", 1) ? true:false;
	adaptiveThreshold() = theApp.GetProfileInt("Options", "adaptiveThreshold", 15);

	leaveResults() = false;

	// Post-processing parms

	gamma() = static_cast<float>(theApp.GetProfileInt("Options", "gamma", 0)) / 100.0f;
	ambient().r() = static_cast<float>(theApp.GetProfileInt("Options", "rAmbient", 0));
	ambient().g() = static_cast<float>(theApp.GetProfileInt("Options", "gAmbient", 0));
	ambient().b() = static_cast<float>(theApp.GetProfileInt("Options", "bAmbient", 0));

	switch(theApp.GetProfileInt("Options", "clamping", RadGen::ClampNone))
	{
		case	ClampSaturate:	clamping() = ClampSaturate;	break;
		case	ClampRetain:	clamping() = ClampRetain;	break;
		default:		clamping() = ClampNone;		break;
	}
    */
}

// ---------------------------------------------------------------------------------------------------------------------------------
// RadGen.cpp - End of file
// ---------------------------------------------------------------------------------------------------------------------------------

#endif // #ifdef ACTIVE_EDITORS