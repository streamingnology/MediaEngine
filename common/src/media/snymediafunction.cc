/*
 *copyleft (c) 2021 www.streamingnology.com
 *code released under GPL license
 */
#include "snymediafunction.h"
#include "Ap4.h"
namespace sny{
SnyUI64 convertTimescale(SnyUI64 time_value, SnyUI64 from_time_scale, SnyUI64 to_time_scale){
  return AP4_ConvertTime(time_value, from_time_scale, to_time_scale);
}
}