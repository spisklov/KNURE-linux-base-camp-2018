#ifndef __LBI_BUTTON_BUTTON_H__
#define __LBI_BUTTON_BUTTON_H__

typedef void (*lbi_on_button_callback_t)(void);

int lbi_create_button(lbi_on_button_callback_t callback);
void lbi_destroy_button(void);

#endif // __LBI_BUTTON_BUTTON_H__
