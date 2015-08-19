function data=frame_load(basedir,prefix,frame)
	filename=sprintf('%s/%s-%d.txt'...
		,basedir,prefix,frame);
	try
		data=dlmread(filename,'\t',1,0);
	catch err
		disp(sprintf('Error loading %s',filename));
		data=[];
	end
end
