function pipe_write(argv,write_callback)
	cmd='';
	for k=1:numel(argv)
		if ~isempty(argv{k})
			cmd=[cmd,'''',argv{k},''' '];
		end
	end
	filename=mkfifo();
	system(['cat ',filename,' | ',cmd,' &']);
	write_callback(filename);
	delete(filename);
end
