function [output]=pipe_read(argv,read_callback)
	cmd='';
	for k=1:numel(argv)
		if ~isempty(argv{k})
			cmd=[cmd,'''',argv{k},''' '];
		end
	end
	filename=mkfifo();
%	system(['cat ',filename,' | ',cmd,' &']);
	system([cmd,' | ','cat',' > ',filename,' & ']);
	output=read_callback(filename);
	delete(filename);
end
