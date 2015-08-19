function [a,b,e_rms]=powerfit(X,Y)
%	Fits an exponential model y=a*x^b to measurement data
	p=polyfit(log(X),log(Y),1);
	a=exp(p(2));
	b=p(1);

	x=fminsearch(@(X_0)(powerfunc(X_0,X,Y)),[a;b]);
	a=x(1);
	b=x(2);
	e_rms=sqrt( powerfunc([a;b],X,Y)/size(X,1) )/(max(Y) - min(Y));
end

function f=powerfunc(X_0,X,Y)
	mod_val=X_0(1).*X.^X_0(2);
	f=sum( (Y - mod_val).^2 );
end
