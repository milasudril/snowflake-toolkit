function prototypechoices_write(file,choices)
	fid=file;
	if isstr(file)
		fid=fopen(file,'w');
	end
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
	if isstr(file)
		fclose(fid);
	end
end