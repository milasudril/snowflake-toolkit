function [a,b,c,e_rms,i_cut]=expfit_tail(X,Y,guess_initial)
%Fits an exponential model
%    y=a exp(bx) + c
%where b<0 to measurement data.

	if(nargin==3)
		i_cut=[];
	%	if(sign(guess_initial(1))>0)
	%		i_cut=find(Y>guess_initial(3));
	%	else
	%		i_cut=find(Y<guess_initial(3));
	%	end
		x=fminunc(@(X_0)(expfunc(X_0,X,Y)),guess_initial);
		a=x(1);
		b=x(2);
		c=x(3);
		e_rms=sqrt( expfunc([a;b;c],X,Y)/size(X,1) )/(max(Y) - min(Y));
	else
	%	Integrate end to find a reference value
		i_0=floor(size(X,1)*7/8)+1;
		i_end=size(X,1);
		end_range=i_0:i_end;
		Y_mid=filter([0.5, 0.5],1,Y(end_range));
		y_ref=Y_mid(2:end)'*diff(X(end_range))/(X(i_end) - X(i_0));
	%%%

	%	Integrate to find the exponential shape, but do not go all the way down
	%	to 0 to avoid any transients
		i_ymax=floor(size(Y,1)/8)+1;
		i_cut=i_ymax:size(Y,1);
		Y_mid=filter([0.5, 0.5],1,Y,0);
		c_sign=sign((Y_mid(i_ymax+1:end) - y_ref)'*diff(X(i_cut)));
	%%%
		c=y_ref*c_sign;
		Y=Y*c_sign;
		[y_max,i_ymax]=max(Y);
		i_cut=i_ymax:size(Y,1);
	%	disp('BEFORE:');
		a=y_max-c;
		b=-1/(0.25*max(X));
		x=fminsearch(@(X_0)(expfunc(X_0,X(i_cut),Y(i_cut))),[a;b;c]);
	%	disp('AFTER:');
		a=c_sign*x(1);
		b=x(2);
		c=c_sign*x(3);
		e_rms=sqrt( expfunc([a;b;c],X,c_sign*Y)/size(X,1) )/(max(Y) - min(Y));
	end
end

function f=expfunc(X_0,X,Y)
	mod_val=X_0(1).*exp(X.*X_0(2)) + X_0(3);
	f=sum( (Y - mod_val).^2 );
end
