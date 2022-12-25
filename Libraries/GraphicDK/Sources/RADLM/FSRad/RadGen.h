// ---------------------------------------------------------------------------------------------------------------------------------
//  _____            _  _____                _     
// |  __ \          | |/ ____|              | |    
// | |__) | __ _  __| | |  __  ___ _ __     | |__  
// |  _  / / _` |/ _` | | |_ |/ _ \ '_ \    | '_ \ 
// | | \ \| (_| | (_| | |__| |  __/ | | | _ | | | |
// |_|  \_\\__,_|\__,_|\_____|\___|_| |_|(_)|_| |_|
//                                                 
//                                                 
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

#ifndef	_H_RADGEN
#define _H_RADGEN

// ---------------------------------------------------------------------------------------------------------------------------------

class	SOctree;
#include "GeomDB.h"

// ---------------------------------------------------------------------------------------------------------------------------------

class	RadGen
{
public:
	// Construction/Destruction

				RadGen()
				: _octreeThreshold(200), _octreeMaxDepth(50), _octreeMinRadius(5.0f), _bspMinSplitRange(5.0f),
				_bspGaussianResolution(8), _leaveResults(false), _writeRawLightmaps(false), _writeOctFile(false),
				_gamma(0), _ambient(0,0,0), _clamping(ClampNone), _convergence(1), _maxIterationsCount(0),
				_maxIterations(false), _ambientTerm(true), _useNusselt(false), _directLightOnly(false),
				_areaLightMultiplier(1000000), _pointLightMultiplier(0.6f), _subdivisionU(4), _subdivisionV(4) {}

		enum		ClampType {ClampNone, ClampSaturate, ClampRetain};

	// Operators

	// Implementation

virtual		void		calcVisiblePieces(const RadPatch & emitter, const RadPrimPointerListGrainy & potentialReceivers, RadPrimListGrainy & visiblePieces);
virtual		void		emit(RadPatch & emitter, const RadPrimPointerListGrainy & potentialReceivers, RadLMapArray & storedEnergy);
virtual		void		emitNusselt(RadPatch & emitter, const RadPrimPointerListGrainy & potentialReceivers, RadLMapArray & storedEnergy);
virtual		void		emitPointLight(RadPatch & emitter, const RadPrimPointerListGrainy & potentialReceivers, RadLMapArray & storedEnergy);
virtual		RadPatch *	findBrightestEmitter();
virtual		bool		processEnergy(SOctree & node);
virtual		geom::Color3	calcTotalEnergy();
virtual		geom::Color3	calcRemainingEnergy();
virtual		bool		updateStats(const bool checkConvergence = true);
virtual		void		distributeInitialEnergy();
virtual		void		countPatchesAndElements();
virtual		geom::Color3	calcAmbientTerm();
virtual		void		expandEdges();
virtual		void		addAmbient();
virtual		void		doGammaCorrection();
virtual		void		doClamping();
virtual		void		go();
virtual		void		readDefaultParms();

	// Accessors

inline		GeomDB &	geometry()				{return _geometry;}
inline	const	GeomDB		geometry() const			{return _geometry;}
inline		unsigned int &	octreeThreshold()			{return _octreeThreshold;}
inline	const	unsigned int	octreeThreshold() const			{return _octreeThreshold;}
inline		unsigned int &	octreeMaxDepth()			{return _octreeMaxDepth;}
inline	const	unsigned int	octreeMaxDepth() const			{return _octreeMaxDepth;}
inline		float &		octreeMinRadius()			{return _octreeMinRadius;}
inline	const	float		octreeMinRadius() const			{return _octreeMinRadius;}
inline		float &		bspMinSplitRange()			{return _bspMinSplitRange;}
inline	const	float		bspMinSplitRange() const		{return _bspMinSplitRange;}
inline		unsigned int &	bspGaussianResolution()			{return _bspGaussianResolution;}
inline	const	unsigned int	bspGaussianResolution()	const		{return _bspGaussianResolution;}
inline		bool &		leaveResults()				{return _leaveResults;}
inline	const	bool		leaveResults()	const			{return _leaveResults;}
inline		bool &		writeRawLightmaps()			{return _writeRawLightmaps;}
inline	const	bool		writeRawLightmaps()	const		{return _writeRawLightmaps;}
inline		fstl::string &	writeRawLightmapsFolder()		{return _writeRawLightmapsFolder;}
inline	const	fstl::string &	writeRawLightmapsFolder()	const	{return _writeRawLightmapsFolder;}
inline		bool &		writeOctFile()				{return _writeOctFile;}
inline	const	bool		writeOctFile()	const			{return _writeOctFile;}
inline		fstl::string &	writeOctFilename()			{return _writeOctFilename;}
inline	const	fstl::string &	writeOctFilename()	const		{return _writeOctFilename;}
inline		fstl::string &	inputFilename()				{return _inputFilename;}
inline	const	fstl::string &	inputFilename()	const			{return _inputFilename;}
inline		float &		gamma()					{return _gamma;}
inline	const	float		gamma() const				{return _gamma;}
inline		geom::Color3 &	ambient()				{return _ambient;}
inline	const	geom::Color3 &	ambient() const				{return _ambient;}
inline		ClampType &	clamping()				{return _clamping;}
inline	const	ClampType	clamping() const			{return _clamping;}
inline		unsigned int &	convergence()				{return _convergence;}
inline	const	unsigned int	convergence() const			{return _convergence;}
inline		unsigned int &	iterationsProcessed()			{return _iterationsProcessed;}
inline	const	unsigned int	iterationsProcessed() const		{return _iterationsProcessed;}
inline		unsigned int &	maxIterationsCount()			{return _maxIterationsCount;}
inline	const	unsigned int	maxIterationsCount() const		{return _maxIterationsCount;}
inline		bool &		maxIterations()				{return _maxIterations;}
inline	const	bool		maxIterations() const			{return _maxIterations;}
inline		bool &		ambientTerm()				{return _ambientTerm;}
inline	const	bool		ambientTerm() const			{return _ambientTerm;}
inline		bool &		useNusselt()				{return _useNusselt;}
inline	const	bool		useNusselt() const			{return _useNusselt;}
inline		bool &		directLightOnly()			{return _directLightOnly;}
inline	const	bool		directLightOnly() const			{return _directLightOnly;}
inline		unsigned int &	areaLightMultiplier()			{return _areaLightMultiplier;}
inline	const	unsigned int	areaLightMultiplier() const		{return _areaLightMultiplier;}
inline		float &		pointLightMultiplier()			{return _pointLightMultiplier;}
inline	const	float		pointLightMultiplier() const		{return _pointLightMultiplier;}
inline		unsigned int &	subdivisionU()				{return _subdivisionU;}
inline	const	unsigned int	subdivisionU() const			{return _subdivisionU;}
inline		unsigned int &	subdivisionV()				{return _subdivisionV;}
inline	const	unsigned int	subdivisionV() const			{return _subdivisionV;}
inline		geom::Color3 &	initialEnergy()				{return _initialEnergy;}
inline	const	geom::Color3 &	initialEnergy() const			{return _initialEnergy;}
inline		geom::Color3 &	totalAbsorbedEnergy()			{return _totalAbsorbedEnergy;}
inline	const	geom::Color3 &	totalAbsorbedEnergy() const		{return _totalAbsorbedEnergy;}
inline		geom::Color3 &	totalEscapedEnergy()			{return _totalEscapedEnergy;}
inline	const	geom::Color3 &	totalEscapedEnergy() const		{return _totalEscapedEnergy;}
inline		geom::Color3 &	energyAbsorbedThisIteration()		{return _energyAbsorbedThisIteration;}
inline	const	geom::Color3 &	energyAbsorbedThisIteration() const	{return _energyAbsorbedThisIteration;}
inline		geom::Color3 &	energyEscapedThisIteration()		{return _energyEscapedThisIteration;}
inline	const	geom::Color3 &	energyEscapedThisIteration() const	{return _energyEscapedThisIteration;}
inline		unsigned int &	adaptiveMaxSubdivisionU()		{return _adaptiveMaxSubdivisionU;}
inline	const	unsigned int	adaptiveMaxSubdivisionU() const		{return _adaptiveMaxSubdivisionU;}
inline		unsigned int &	adaptiveMaxSubdivisionV()		{return _adaptiveMaxSubdivisionV;}
inline	const	unsigned int	adaptiveMaxSubdivisionV() const		{return _adaptiveMaxSubdivisionV;}
inline		bool &		adaptivePatchSubdivision()		{return _adaptivePatchSubdivision;}
inline	const	bool		adaptivePatchSubdivision() const	{return _adaptivePatchSubdivision;}
inline		unsigned int &	adaptiveThreshold()			{return _adaptiveThreshold;}
inline	const	unsigned int	adaptiveThreshold() const		{return _adaptiveThreshold;}
inline		unsigned int &	totalPatches()				{return _totalPatches;}
inline	const	unsigned int	totalPatches() const			{return _totalPatches;}
inline		unsigned int &	totalElements()				{return _totalElements;}
inline	const	unsigned int	totalElements() const			{return _totalElements;}
inline		float &		energyThisPass()			{return _energyThisPass;}
inline	const	float		energyThisPass() const			{return _energyThisPass;}
inline		float &		uTexelsPerUnit()			{return _uTexelsPerUnit;}
inline	const	float		uTexelsPerUnit() const			{return _uTexelsPerUnit;}
inline		float &		vTexelsPerUnit()			{return _vTexelsPerUnit;}
inline	const	float		vTexelsPerUnit() const			{return _vTexelsPerUnit;}
inline		unsigned int &	lightmapWidth()				{return _lightmapWidth;}
inline	const	unsigned int	lightmapWidth() const			{return _lightmapWidth;}
inline		unsigned int &	lightmapHeight()			{return _lightmapHeight;}
inline	const	unsigned int	lightmapHeight() const			{return _lightmapHeight;}
inline		geom::Color3 &	defaultReflectivity()			{return _defaultReflectivity;}
inline	const	geom::Color3	defaultReflectivity() const		{return _defaultReflectivity;}
inline		bool &		lightsToProcess()			{return _lightsToProcess;}
inline	const	bool		lightsToProcess() const			{return _lightsToProcess;}

private:
	// Data members

		GeomDB		_geometry;
		unsigned int	_octreeThreshold;
		unsigned int	_octreeMaxDepth;
		float		_octreeMinRadius;
		float		_bspMinSplitRange;
		unsigned int	_bspGaussianResolution;
		bool		_leaveResults;
		bool		_writeRawLightmaps;
		fstl::string	_writeRawLightmapsFolder;
		bool		_writeOctFile;
		fstl::string	_writeOctFilename;
		fstl::string	_inputFilename;
		float		_gamma;
		geom::Color3	_ambient;
		ClampType	_clamping;
		unsigned int	_convergence;
		unsigned int	_iterationsProcessed;
		unsigned int	_maxIterationsCount;
		bool		_maxIterations;
		bool		_ambientTerm;
		bool		_useNusselt;
		bool		_directLightOnly;
		unsigned int	_areaLightMultiplier;
		float		_pointLightMultiplier;
		unsigned int	_subdivisionU;
		unsigned int	_subdivisionV;
		geom::Color3	_initialEnergy;
		geom::Color3	_totalAbsorbedEnergy;
		geom::Color3	_totalEscapedEnergy;
		geom::Color3	_energyAbsorbedThisIteration;
		geom::Color3	_energyEscapedThisIteration;
		unsigned int	_adaptiveMaxSubdivisionU;
		unsigned int	_adaptiveMaxSubdivisionV;
		bool		_adaptivePatchSubdivision;
		unsigned int	_adaptiveThreshold;
		unsigned int	_totalPatches;
		unsigned int	_totalElements;
		float		_energyThisPass;
		float		_uTexelsPerUnit;
		float		_vTexelsPerUnit;
		unsigned int	_lightmapWidth;
		unsigned int	_lightmapHeight;
		geom::Color3	_defaultReflectivity;
		bool		_lightsToProcess;
};

#endif // _H_RADGEN
// ---------------------------------------------------------------------------------------------------------------------------------
// RadGen.h - End of file
// ---------------------------------------------------------------------------------------------------------------------------------
