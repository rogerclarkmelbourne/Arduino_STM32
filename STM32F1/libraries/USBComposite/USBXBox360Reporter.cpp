/* Copyright (c) 2011, Peter Barrett
**
** Permission to use, copy, modify, and/or distribute this software for
** any purpose with or without fee is hereby granted, provided that the
** above copyright notice and this permission notice appear in all copies.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
** SOFTWARE.
*/

#include <Arduino.h>
#include "USBComposite.h" 

bool USBXBox360Reporter::wait() {
    uint32_t t=millis();
	while (x360_is_transmitting(controller) != 0 && (millis()-t)<500) ;
    return ! x360_is_transmitting(controller);
}

bool USBXBox360Reporter::sendData(const void* data, uint32 length){
    if (wait()) {
        x360_tx(controller, (uint8*)data, length);

        if(wait()) {
        /* flush out to avoid having the pc wait for more data */
            x360_tx(controller, NULL, 0);
        }
        return true;
    }
    return false;
}

void USBXBox360Reporter::setManualReportMode(bool mode) {
    manualReport = mode;
}

bool USBXBox360Reporter::getManualReportMode() {
    return manualReport;
}

