function [output]=pipe_read(argv,read_callback)
	cmd='';
	for k=1:numel(argv)
		if ~isempty(argv{k})
			cmd=[cmd,'''',argv{k},''' '];
		end
	end
	if is_octave
		filename=mkfifo();
		system([cmd,' | ','cat',' > ',filename,' & ']);
		output=read_callback(filename);
		delete(filename);
	else
	%	FIFO broken in matlab?

		rng=fopen('/dev/urandom','r');
		x=fread(rng,1,'uint32');
		y=fread(rng,3,'uint16');
		z=fread(rng,3,'uint16');
		fclose(rng);
		filename=sprintf('/tmp/%08x-%04x-%04x-%04x-%04x%04x%04x',x,y(1),y(2),y(3),z(1),z(2),z(3));
		system([cmd,' | ','cat',' > ',filename]);
		output=read_callback(filename);
		delete(filename);
	end
end

function r = is_octave ()
	persistent x;
	if (isempty (x))
		x = exist ('OCTAVE_VERSION', 'builtin');
	end
	r = x;
end