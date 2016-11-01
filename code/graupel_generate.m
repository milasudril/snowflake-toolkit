function graupel_generate(paramstruct)
	seed=['--seed=',int2str(paramstruct.seed)];
	scale=['--scale=[',num2str(paramstruct.mean),',',num2str(paramstruct.std),']']

end