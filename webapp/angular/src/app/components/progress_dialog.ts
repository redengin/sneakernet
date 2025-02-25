import { Component } from '@angular/core';

import { MatProgressSpinnerModule } from '@angular/material/progress-spinner';

@Component({
    templateUrl: './progress_dialog.html',
    imports: [MatProgressSpinnerModule],
})
export class ProgressDialog {
};