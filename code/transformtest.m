V0=2*(rand(3,1)-0.5);
V1=2*(rand(3,1)-0.5);
V2=2*(rand(3,1)-0.5);

T=[V0(1),V0(2),V0(3);
	V1(1),V1(2),V1(3);
	V2(1),V2(2),V2(3);
	V0(1),V0(2),V0(3)]

plot3(T(:,1),T(:,2),T(:,3),'.-');
hold on

S=V1-V0;
T=V2-V0;
N=cross(S,T);

A=[S,T,N]

for k=1:17
	for l=1:17
		s=(k-1)/16;
		t=(l-1)/16;
		if t <= 1-s
			r=A*[s;t;0] + V0;
			plot3(r(1),r(2),r(3),'r.');
		end
	end
end
hold off
xlabel('x');
ylabel('y');
zlabel('z');

norm(V2-V1)

a=inv(A)*(V1 - V0)
b=inv(A)*(V2 - V0)
norm(A*(a - b))