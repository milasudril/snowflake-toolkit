function [stats]=graupel_generate(paramstruct,exepath,exefile)
% function [stats]=graupel_generate(paramstruct,exepath,exefile)
%
% Recognized members of paramstruct
%
% seed               seed for the random generator
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
% statefile          restarts a simuation stored in a statefile
% dump_geometry      generate a Wavefront file from the given geometry
% dump_geometry_ice  save ball radius and positions to a file, so it can be processed by
%                    sphere_aggreagte_rasterize
% dump_stats         write statistics to the given file
%
% For a more detailed description, run `exepath/exefile --help` from a 
% terminal. If exefile is ommited, the function will start graupel_generate2
%
	seed=ternary(@()isfield(paramstruct,'seed')...
		,@()['--seed=',int2str(paramstruct.seed)]...
		,@()'');

	scale=ternary(@()isfield(paramstruct,'scale')...
		,@()['--scale=[',num2str(paramstruct.scale.mean),',',num2str(paramstruct.scale.std),']']...
		,@()'');

	E_0=ternary(@()isfield(paramstruct,'E_0')...
		,@()['--E_0=',num2str(paramstruct.E_0)]...
		,@()'');

	decay_distance=ternary(@()isfield(paramstruct,'decay_distance')...
		,@()['--decay-distance=',num2str(paramstruct.decay_distance)]...
		,@()'');

	merge_offset=ternary(@()isfield(paramstruct,'merge_offset')...
		,@()['--merge-offset=',num2str(paramstruct.merge_offset)]...
		,@()'');

	overlap_max=ternary(@()isfield(paramstruct,'overlap_max')...
		,@()['--overlap-max=',int2str(paramstruct.overlap_max)]...
		,@()'');

	D_max=ternary(@()isfield(paramstruct,'D_max')...
		,@()['--D_max=',num2str(paramstruct.D_max)]...
		,@()'');

	fill_ratio=ternary(@()isfield(paramstruct,'fill_ratio')...
		,@()['--fill-ratio=',num2str(paramstruct.fill_ratio)]...
		,@()'');

	statefile=ternary(@()isfield(paramstruct,'statefile')...
		,@()['--statefile=',paramstruct.statefile]...
		,@()'');

	dump_geometry=ternary(@()isfield(paramstruct,'dump_geometry')...
		,@()['--dump-geometry=',paramstruct.dump_geometry]...
		,@()'');


	dump_geometry_ice=ternary(@()isfield(paramstruct,'dump_geometry_ice')...
		,@()['--dump-geometry-ice=',paramstruct.dump_geometry_ice]...
		,@()'');

	dump_stats=ternary(@()isfield(paramstruct,'dump_stats')...
		,@()['--dump-stats=',paramstruct.dump_stats]...
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

	system_wrapper({cmd,seed,scale,E_0,decay_distance,merge_offset...
		,overlap_max,D_max,fill_ratio,statefile,dump_geometry,dump_geometry_ice...
		,dump_stats},nargout()>0);
	if ~isempty(dump_stats) && nargout()>0
		stats=csvread2(paramstruct.dump_stats,'\t');
	end
end
