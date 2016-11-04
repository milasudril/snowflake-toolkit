function [a,b,e_rms]=linfit(X,Y)
%	Fits a linear model y=ax + b to measurement data
	p=polyfit(X,Y,1);
	a=p(1);
	b=p(2);

	e_rms=sqrt( linfun([a;b],X,Y)/size(X,1) )/(max(Y) - min(Y));
end

function f=linfun(X_0,X,Y)
	mod_val=X_0(1).*X + X_0(2);
	f=sum( (Y - mod_val).^2 );
end
