function vols=ice_loader(source,exepath)
	vols={};
	n=nargin();
	cmd=ternary(@()(n<2),@()'snowflake_prototype-test'...
		,@()[exepath,'/snowflake_prototype-test']);
	f='test.txt';   %mkfifo();
	system_wrapper({cmd,['--dump-geometry-simple=',f],['--prototype=',source]},1);
	fid=fopen(f,'r');
	line=fgetl(fid);
	if line(1)~='V'
		fclose(fid);
		delete(f);
		return
	end

	vols{end+1}=struct('vertices',[],'faces',[]);
	while ischar(line)
		line=fgetl(fid);
		while ischar(line)
			if line(1)=='f'
				line=fgetl(fid);
				break
			end
			strings=strsplit(line);
			vols{end}.vertices(:,end+1)=[str2num(strings{1});str2num(strings{2});str2num(strings{3})];
			line=fgetl(fid);
		end
		while ischar(line)
			if line(1)=='V'
				vols{end+1}=struct('vertices',[],'faces',[]);
				break
			end
			strings=strsplit(line);
			vols{end}.faces(:,end+1)=[str2num(strings{1});str2num(strings{2});str2num(strings{3})];
			line=fgetl(fid);
		end
	end
	fclose(fid);
%	delete(f);
end
