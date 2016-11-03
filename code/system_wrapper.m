function system_wrapper(argv,block)
	cmd='';
	for k=1:numel(argv)
		if ~isempty(argv{k})
			cmd=[cmd,'''',argv{k},''' '];
		end
	end
	disp(cmd);
	if block
		system(cmd);
	else
		system([cmd,' &']);
	end
end
