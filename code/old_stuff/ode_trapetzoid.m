function [T,Y]=ode_trapetzoid(odefun,t_vals,Y_0)
	T=t_vals;
	Y=zeros(size(t_vals,1),size(Y_0,1));
	Y(1,:)=Y_0';
	dt=diff(t_vals);
	time=0;
	o=optimset('Display','off');
	for k=2:size(t_vals,1)
		h=dt(k-1);
		y_est=Y(k-1,:)+odefun(time,Y(k-1,:)',k-1)'*h;
		time=time+h;
		Y(k,:)=(fsolve(@(y_1)(y_1 - Y(k-1,:) - h*odefun( time, y_1,k)'),y_est,o) + y_est)/2;
	end
end
