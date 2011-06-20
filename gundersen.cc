
/*
  Computing ($pT$):
  Let $\T \in R^{n\times n\times n}$ be a tensor.
  Let $\M \in R^{n\times n}$ be a matrix.
  Let $p \in R^{n}$ be a vector.
  for i = 1 to n do
    for j = 1 to i - 1 do 
      for k = 1 to j − 1 do
        M[i][j] += p[k] * T[i][j][k]
        M[i][k] += p[j] * T[i][j][k]
        M[j][k] += p[i] * T[i][j][k]
      end for
      M[i][j] += p[j] * T[i][j][j]
      M[j][j] += p[i] * T[i][j][j]
    end for
    for k = 1 to i - 1 do 
      M[i][i] += p[k] * T[i][i][k]
      M[i][k] += p[i] * T[i][i][k]
    end for
    M[i][i] += p[i] * T[i][i][i]
  end for
*/

void
gundersen()
{
  //The Super-Symmetric Compressed Tube Storage (CTS) of the induced tensor:
  double valueT[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19}; 
  int indexT[]	= {0,0,0,1,2,1,1,3,2,2,4,0,2,2,4,0,2,4,5}; 
  int pointerT[] = {0,1,2,4,5,6,8,9,11,12,13,15,19};
  
  // The operation g \gets g + (pT)p using the above data structures:
  int start = 0, stop = 0, i = 0, j = 0, k = 0; 
  int ind = 0, tp = 0; 
  int pi = 0, pj = 0, pk = 0, pipj = 0, kpipj = 0, kpi = 0, kpj = 0, pipi = 0, pkTijk=0; 
  int startTubek=0, stopTubek=0; 
  double Tijk = 0, Tijj = 0, Tiii = 0, Tiik = 0; 
  double ga = 0; 
  
  for(i = 0;i<N;i++,ind++,tp++){
    start = pointerH[i]; 
    j = indexH[start]; 
    pi = p[i]; 
    kpi = 2*pi;
    pipi = pi*pi; 
    for(;j<i;start++,j=indexH[start],ind++,tp++){
      pj = p[j]; 
      kpj = 2*pj; 
      kpipj = pj*kpi; 
      startTubek = pointerT[tp]; 
      stopTubek = pointerT[tp+1]-1; 
      for(;startTubek<stopTubek;startTubek++,ind++){
	//Handle the case when no indices are equal: i!=j!=k!=i 
	Tijk = valueT[ind]; 
	pkTijk = p[indexT[ind]]*Tijk; 
	ga += pkTijk;
	g[k] += kpipj*Tijk;
      } 
      //Handle the case when two indices are equal: k=j 
      Tijj = valueT[ind]; 
      g[i] += (kpj*ga+pj*pj*Tijj); 
      g[j] += (kpi*ga+kpipj*Tijj); 
      ga = 0.0;
    } 
    startTubek = pointerT[tp]; 
    stopTubek = pointerT[tp+1]-1; 
    for(;startTubek<stopTubek;startTubek++,ind++){
      //Handle the case when two indices are equal: j=i 
      Tiik = valueT[ind]; 
      ga += p[indexT[ind]]*Tiik; 
      g[k] += pipi*Tiik;
    } 
    //Handle the case when all three indices are equal 
    g[i] += (kpi*ga + pipi*valueT[ind]); 
    ga = 0.0;
  }
}
