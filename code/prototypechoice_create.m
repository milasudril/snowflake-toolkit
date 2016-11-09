function choice=prototypechoice_create(prototype,probability,deformations)
% function choice=prototypechoice_create(prototype,probability,deformations)
%
% Creates a prototype choice record
%
% A `prototype` (refered to by a filename) is chosen with `probability`.
% Given that prototype, all of the listed deformations are applied.
% A deformation is a cell array with elements parameter and distribution,
% followed by arguments. Deformation can for example take the form
%
%  {{'L','gamma',1,0.25},{'a','custom','distribution_file.txt'}}
%
% Here distribution_file.txt is a tab delimited text file with the first
% column containing the value, and the second containing the frequency.
%
	choice=struct('prototype',prototype,'probability',probability,'deformations',{{}});
	for k=1:numel(deformations)
		choice.deformations{k}=deformations{k};
	end
end
