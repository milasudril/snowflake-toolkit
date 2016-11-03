function vols=ice_load(source,exepath)
	n=nargin();
	vols={};
	cmd=ternary(@()(n<2),@()'snowflake_prototype-test'...
		,@()[exepath,'/snowflake_prototype-test']);
	f=mkfifo();
	system_wrapper({cmd,['--dump-geometry-simple=',f],['--prototype=',source]},0);
	data=dlmread(f,' ');
	vol_current=0;
	while 1
		ind=find(data(:,1)==vol_current);
		if isempty(ind)
			return;
		end
		v_rows=find(data(:,2)==0 & data(:,1)==vol_current);
		v_faces=find(data(:,2)==1 & data(:,1)==vol_current);
		vol_current=vol_current+1;
		vols{vol_current}=struct('vertices',data(v_rows,3:5)...
			,'faces',1 + data(v_faces,3:5));
	end
	delete(f);
end
