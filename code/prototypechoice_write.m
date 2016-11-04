function prototypechoice_write(fid,prototype_name,probability,deformations)
	fprintf(fid,'{"prototype":"%s","probability":%.8g,"deformations":[',prototype_name,probability);
	for k=1:numel(deformations)
		deformation=deformations{k};
		if k~=1
			fprintf(fid,',');
		end
		fprintf(fid,'["%s","%s"',deformation{1},deformation{2});
		for l=3:numel(deformation)
			fprintf(fid,',%.8g',deformation{l});
		end
		fprintf(fid,']');
	end
	fprintf(fid,']}\n');
end
