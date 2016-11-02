function system_wrapper(argv)
	cmd='';
	for k=1:numel(argv)
		if ~isempty(argv{k})
			cmd=[cmd,'''',argv{k},''' '];
		end
	end
	system(cmd);
end
