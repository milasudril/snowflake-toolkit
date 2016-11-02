function choice=prototypechoice_create(prototype,probability,deformations)
	choice=struct('prototype',prototype,'probability',probability,'deformations',{{}});

	for k=1:numel(deformations)
		choice.deformations{k}=deformations{k};
	end
end
