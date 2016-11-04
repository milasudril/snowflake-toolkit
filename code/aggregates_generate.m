function aggregates_generate(paramstruct,sync,exepath,exefile)
% function aggregates_generate(paramstruct,sync,exepath,exefile)
%
% Recognized members of paramstruct
% 
% statefile_in       restarts a simuation stored in a statefile
%
% statefile_out      when exiting, save simuation state to statefile
%
% stop_cond          defines a stop condition
%
% shape              selects a crystal prototype for use during the simulation
%
% deformations       adds deformations to the crystal prototype given by `shape`.
%                    `deformations` is given as cell array, containing all 
%                    deformation rules. For more information, see 
%                    prototypechoice_create.
%
% prototype_choices  choose prototypes from a set. This member may either be a
%                    string refering to a file stored on disk, or it may be a
%                    cell array of objectcs created by prototypechoice_create.
% 
% output_dir         sets the directory for output data
%
% dump_stats         writes particle measurements every N iteration
% 
% dump_geometry      exports the output geometry as Wavefront files
% 
% dump_geometry_ice  same as dump_geometry but write data as Ice crystal prototype
%                    files
% 
% sample_geometry    samples the output geometry to a grid. This member is expected
%                    to be an array with the the elements Nx, Ny, Nz.
% 
% seed               sets the random number genererator seed
% 
% N                  defines the maximum number of crystals
% 
% droprate           Model parameter
% 
% growthrate         Model parameter
% 
% meltrate           Model parameter
% 
% merge_retries      If the first try to merge two aggregates failed to satisfy
%                    the overlap constraint, try again N times without skipping
%                    the current event.
% 
% overlap_min        When merging particles, only accept events where the ratio
%                    of overlapping subvolumes is greater than ratio.
% 
% overlap_max        When merging particles, only accept events where the ratio
%                    of overlapping subvolumes is less than ratio.
%
% For a more detailed description, run `exepath/exefile --help` from a 
% terminal. If exefile is ommited, the function will start snowflake_simulate3

	[~,nowstring]=system('date --rfc-3339=ns --utc');
	nowstring=strtrim(nowstring);
	nowstring=strsplit(nowstring,'+');
	nowstring=nowstring{1};
	nowstring=regexprep(nowstring,'\s','_');
	nowstring=regexprep(nowstring,'[:+.]','-');

	statefile_in=ternary(@()isfield(paramstruct,'statefile_in')...
		,@()['--statefile-in=',paramstruct.statefile_in]...
		,@()'');

	statefile_out=ternary(@()isfield(paramstruct,'statefile_out')...
		,@()['--statefile-out=',paramstruct.statefile_out]...
		,@()'');

	stop_cond=ternary(@()isfield(paramstruct,'stop_cond')...
		,@()['--stop-cond=',paramstruct.stop_cond]...
		,@()'');

	shape=ternary(@()isfield(paramstruct,'shape')...
		,@()['--shape=',paramstruct.shape]...
		,@()'');

	deformations={};
	if isfield(paramstruct,'deformations')
		for k=1:numel(paramstruct.deformations)
			def=paramstruct.deformations{k};
			str=['--deformation=',def{1},',',def{2}];
			for l=3:numel(def)
				str=[str,',',num2str(def{l})];
			end
			deformations{k}=str;
		end
	end

	prototype_choices='';
	if isfield(paramstruct,'prototype_choices')
		if ischar(paramstruct.prototype_choices)
			prototype_choices=['--prototype-choises=',paramstruct.prototype_choices];
		else
			choices_file=['choices_',nowstring,'.json'];
			prototypechoices_write(choices_file,paramstruct.prototype_choices);
			prorotype_choices=['--prototype-choises=',choices_file];
		end
	end

	output_dir=ternary(@()isfield(paramstruct,'output_dir')...
		,@()['--output-dir=',paramstruct.output_dir]...
		,@()'');

	dump_stats=ternary(@()isfield(paramstruct,'dump_stats')...
		,@()['--dump-stats=',num2str(paramstruct.dump_stats)]...
		,@()'');

	dump_geometry=ternary(@()isfield(paramstruct,'dump_geometry')...
		,@()makebool('--dump-geometry',paramstruct.dump_geometry)...
		,@()'');

	dump_geometry_ice=ternary(@()isfield(paramstruct,'dump_geometry_ice')...
		,@()makebool('--dump-geometry-ice',paramstruct.dump_geometry_ice)...
		,@()'');

	sample_geometry=ternary(@()isfield(paramstruct,'sample_geometry')...
		,@()['--sample_gemetry=',num2str(paramstruct.sample_geometry(1)),','...
				,num2str(paramstruct.sample_geometry(2)),','...
				,num2str(paramstruct.sample_geometry(3))] ...
		,@()'');

	seed=ternary(@()isfield(paramstruct,'seed')...
		,@()['--seed=',int2str(paramstruct.seed)]...
		,@()'');

	N=ternary(@()isfield(paramstruct,'N')...
		,@()['--N=',num2str(paramstruct.N)]...
		,@()'');

	droprate=ternary(@()isfield(paramstruct,'droprate')...
		,@()['--droprate=',num2str(paramstruct.droprate)]...
		,@()'');

	meltrate=ternary(@()isfield(paramstruct,'meltrate')...
		,@()['--meltrate=',num2str(paramstruct.meltrate)]...
		,@()'');

	growthrate=ternary(@()isfield(paramstruct,'growthrate')...
		,@()['--growthrate=',num2str(paramstruct.growthrate)]...
		,@()'');

	merge_retries=ternary(@()isfield(paramstruct,'merge_retries')...
		,@()['--merge-retries=',num2str(paramstruct.merge_retries)]...
		,@()'');

	overlap_min=ternary(@()isfield(paramstruct,'overlap_min')...
		,@()['--overlap-min=',num2str(paramstruct.overlap_min)]...
		,@()'');

	overlap_max=ternary(@()isfield(paramstruct,'overlap_max')...
		,@()['--overlap-max=',int2str(paramstruct.overlap_max)]...
		,@()'');

	n=nargin();
	cmd='';
	switch nargin()
		case 1
			cmd='snowflake_simulate3';
		case 2
			cmd='snowflake_simulate3';
		case 3
			cmd=[exepath,'/snowflake_simulate3'];
		otherwise
			cmd=[exepath,'/',exefile];
	end

	system_wrapper({cmd,statefile,stop_cond,shape,deformations...
		,prototype_choices,output_dir,dump_stats,dump_geometry...
		,dump_geometry_ice,sample_geometry,seed,N,growthrate...
		,droprate,meltrate,merge_retries,overlap_min,overlap_max}...
		,sync);
end

function res=makebool(str,value)
	res=ternary(@()(value==1),@()(str),@()(''));
end
