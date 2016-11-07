function vols=ice_load(source,exepath)
	n=nargin();
	vols={};
	cmd='';
	if n<2
		cmd='snowflake_prototype-test';
	else
		cmd=[exepath,'/snowflake_prototype-test'];
	end
	data=pipe_read({cmd,'--dump-geometry-simple',['--prototype=',source]}...
		,@(filename)(dlmread(filename,' ',1,0)));
	vol_current=0;
	slice=1;
	sections=[0;diff(data(:,1))];
	delims=[find(sections),size(data,1)];
	for vol_current=1:numel(delims)
		slice_start=slice;
		slice=delims(vol_current);
		v_rows=find(data(slice_start:slice,2)==0);
		v_faces=find(data(slice_start:slice,2)==1);
		vols{vol_current}=struct('vertices',data(v_rows+slice_start-1,3:5)...
			,'faces',1 + data(v_faces+slice_start-1,3:5));
	end
end
