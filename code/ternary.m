function result=ternary(cond,if_true,if_false)
	if cond()
		result=if_true();
	else
		result=if_false();
	end
end
