function prototypechoices_write(fid,choices)
	fprintf(fid,'[\n');
	for k=1:numel(choices)
		if k~=1
			fprintf(fid,',');
		else
			fprintf(fid,' ');
		end
		prototypechoice_write(fid,choices{k}.prototype,choices{k}.probability ...
			,choices{k}.deformations);
	end
	fprintf(fid,']');
end