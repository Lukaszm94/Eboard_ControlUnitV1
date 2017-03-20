/*
 * powerswitchmanager.h
 *
 *  Created on: 28 gru 2016
 *      Author: Luke
 */

#ifndef POWERSWITCHMANAGER_H_
#define POWERSWITCHMANAGER_H_

void ps_init(void);
void ps_enable(void);
void ps_disable(void);
int ps_isEnabled(void);

#endif /* POWERSWITCHMANAGER_H_ */
