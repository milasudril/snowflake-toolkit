function prototypechoices_write(file,choices)
% Stores the prototype choices to file, assuming choices is a cell array of 
% prototype choices. A prototype choice can be created by using the function
% prototypechoice_create.
%
	fid=file;
	if ischar(file)
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
	if ischar(file)
		fclose(fid);
	end
end