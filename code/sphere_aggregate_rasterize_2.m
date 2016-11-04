function coords=sphere_aggregate_rasterize_2(aggregate,n_points,ratio,output_file,exepath,exename)
% function coords=sphere_aggregate_rasterize_2(aggregate,n_points,ratio,output_file,exepath,exename)
%
% Rasterizes ice_file, using N points, with ratio [Rx,Ry,Rz]
%
	prototype=['--source=',aggregate];
	sample_geometry=['--grid2=[',num2str(n_points),','];
	for k=1:numel(ratio)
		sample_geometry=[sample_geometry,num2str(ratio(k))...
			,ternary(@()(k==numel(ratio)), @()(','),@()(':')) ];
	end
	sample_geometry=[sample_geometry,output_file,']'];

	n=nargin();
	cmd='';
	switch nargin()
		case 1
			cmd='sphere_aggregate_rasterize';
		case 2
			cmd='sphere_aggregate_rasterize';
		case 3
			cmd='sphere_aggregate_rasterize';
		case 4
			cmd='sphere_aggregate_rasterize';
		case 5
			cmd=[exepath,'/sphere_aggregate_rasterize'];
		otherwise
			cmd=[exepath,'/',exefile];
	end

	system_wrapper({cmd,prototype,sample_geometry},nargout()>0);
	if nargout()>0
		tmp=importdata(output_file,' ');
		coords=tmp.data;
	end
end
