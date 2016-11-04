function [V]=odesolve_test(t,v,index)
	x=0;
	if(index>32)
		x=1;
	end
	omega=150;
	V=[v(2);omega^2*x-v(1)*omega^2-0.5*omega*v(2)];
end
