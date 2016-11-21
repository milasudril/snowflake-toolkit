function [stats]=graupel_generate(paramstruct,exepath,exefile)
% function [stats]=graupel_generate(paramstruct,exepath,exefile)
%
% Generates a graupel
%
% IN
%	paramstruct	A struct with simulation parameters
%	exedir	exepath the directory containing the executable for running
%		the simulation
%	exefile	the name of the executable file
%
% OUT
%	stats	Growth history of the generated graupel
%
% Recognized members of paramstruct
%
% seed               seed for the random generator
% beam_width         the width of injector beam
% pmap               direction probability map stored in a matrix
% projection         projection mode, either cylindrical (this is default), or raw
% zenith_adjust      enable/disable zenith probability adjustment
% scale              scaling of the spheres. It is given with a struct with the members `mean`,
%                    and `std`
% E_0                initial energy
% decay_distance     distance a sphere must travel before its energy has dropped to 1/e
% merge_offset       merging distance at initial point. A positive value pulls the two 
%                    particles together
% overlap_max        the maximum number of overlaps to accept, including the reference 
%                    particle
% D_max              sets a desired D_max
% fill_ratio         sets a desired fill ratio. If fill ratio is not fullfilled when the
%                    graupel reaches the size D_max, continue, but only accept events that
%                    would not increase D_max
% statefile_in       restarts a simuation stored in a statefile
% statefile_out      saves simulation state to a statefile
% dump_geometry      generate a Wavefront file from the given geometry
% dump_geometry_ice  save ball radius and positions to a file, so it can be processed by
%                    sphere_aggreagte_rasterize
% dump_stats         write statistics to the given file
%
% For a more detailed description, run `exepath/exefile --help` from a 
% terminal. If exefile is ommited, the function will start graupel_generate2
%
	seed=ternary(@()( isfield(paramstruct,'seed') && ~isempty(paramstruct.seed))...
		,@()['--seed=',int2str(paramstruct.seed)]...
		,@()'');

	beam_width=ternary(@()( isfield(paramstruct,'beam_width') && ~isempty(paramstruct.seed))...
		,@()['--beam-width=',num2str(paramstruct.beam_width)]...
		,@()'');

	rng=fopen('/dev/urandom','r');
	x=fread(rng,1,'uint32');
	y=fread(rng,3,'uint16');
	z=fread(rng,3,'uint16');
	fclose(rng);

	pmap_name=sprintf('/tmp/%08x-%04x-%04x-%04x-%04x%04x%04x.png',x,y(1),y(2),y(3),z(1),z(2),z(3));
	pmap=ternary(@()( isfield(paramstruct,'pmap') && ~isempty(paramstruct.pmap) )...
		,@()['--pmap=',pmap_name]...
		,@()'');

	projection=ternary(@()( isfield(paramstruct,'projection') && ~isempty(paramstruct.projection) )...
		,@()['--projection=',paramstruct.projection]...
		,@()['']);

	zenith_adjust=ternary(@()( isfield(paramstruct,'zenith_adjust') && ~isempty(paramstruct.zenith_adjust) )...
		,@()['--zenith-adjust=',truefalse(paramstruct.projection>0)]...
		,@()['']);

	scale=ternary(@()( isfield(paramstruct,'scale') && ~isempty(paramstruct.scale) )...
		,@()['--scale=[',num2str(paramstruct.scale.mean),',',num2str(paramstruct.scale.std),']']...
		,@()'');

	E_0=ternary(@()( isfield(paramstruct,'E_0') && ~isempty(paramstruct.E_0) )...
		,@()['--E_0=',num2str(paramstruct.E_0)]...
		,@()'');

	decay_distance=ternary(@()( isfield(paramstruct,'decay_distance') && ~isempty(paramstruct.decay_distance) )...
		,@()['--decay-distance=',num2str(paramstruct.decay_distance)]...
		,@()'');

	merge_offset=ternary(@()( isfield(paramstruct,'merge_offset') && ~isempty(paramstruct.merge_offset) )...
		,@()['--merge-offset=',num2str(paramstruct.merge_offset)]...
		,@()'');

	overlap_max=ternary(@()( isfield(paramstruct,'overlap_max') && ~isempty(paramstruct.overlap_max) )...
		,@()['--overlap-max=',int2str(paramstruct.overlap_max)]...
		,@()'');

	D_max=ternary(@()( isfield(paramstruct,'D_max') && ~isempty(paramstruct.D_max) )...
		,@()['--D_max=',num2str(paramstruct.D_max)]...
		,@()'');

	fill_ratio=ternary(@()( isfield(paramstruct,'fill_ratio') && ~isempty(paramstruct.fill_ratio))...
		,@()['--fill-ratio=',num2str(paramstruct.fill_ratio)]...
		,@()'');

	statefile_in=ternary(@()( isfield(paramstruct,'statefile_in') && ~isempty(paramstruct.statefile_in) )...
		,@()['--statefile-in=',paramstruct.statefile_in]...
		,@()'');

	statefile_out=ternary(@()( isfield(paramstruct,'statefile_out') && ~isempty(paramstruct.statefile_out) )...
		,@()['--statefile-out=',paramstruct.statefile_out]...
		,@()'');

	dump_geometry=ternary(@()( isfield(paramstruct,'dump_geometry') && ~isempty(paramstruct.dump_geometry) )...
		,@()['--dump-geometry=',paramstruct.dump_geometry]...
		,@()'');


	dump_geometry_ice=ternary(@()( isfield(paramstruct,'dump_geometry_ice') && ~isempty(paramstruct.dump_geometry_ice) )...
		,@()['--dump-geometry-ice=',paramstruct.dump_geometry_ice]...
		,@()'');

	dump_stats=ternary(@()( isfield(paramstruct,'dump_stats') && ~isempty(paramstruct.dump_stats) )...
		,@()['--dump-stats=',paramstruct.dump_stats]...
		,@()'');

	report_rate=ternary(@()( isfield(paramstruct,'report_rate') && ~isempty(paramstruct.report_rate) )...
		,@()['--report-rate=',paramstruct.report_rate]...
		,@()'');

	n=nargin();
	cmd='';
	switch nargin()
		case 1
			cmd='graupel_generate2';
		case 2
			cmd=[exepath,'/graupel_generate2'];
		otherwise
			cmd=[exepath,'/',exefile];
	end

	if ~isempty(pmap)
		imwrite(paramstruct.pmap,pmap_name);
	end

	system_wrapper({cmd,seed,scale,E_0,decay_distance,merge_offset...
		,overlap_max,D_max,fill_ratio,statefile_in,statefile_out...
		,dump_geometry,dump_geometry_ice...
		,dump_stats,report_rate,pmap,projection,beam_width,zenith_adjust},nargout()>0);
	if ~isempty(dump_stats) && nargout()>0
		stats=csvread2(paramstruct.dump_stats,'\t');
	end
end

function str=truefalse(x)
	if x
		str='yes'
	else
		str='no'
	end
end
