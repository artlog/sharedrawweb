#ifndef __BEZIER_H__
#define __BEZIER_H__

/*
coeff[0] (1-t)³
coeff[1] 3.t.(1-t)²
coeff[2] 3.t².(1-t)
coeff[3] t³
*/
struct bezier_cubic_param {
  double coeff[4];
};

/*
   compute coeff for t=(index/points)
 */
void calc_coeff(struct bezier_cubic_param * param, int index, int points);

/* 
   compute XPoint given params
*/
void calc_XPoint(struct bezier_cubic_param * param, XPoint p[4], XPoint * result);

#endif
