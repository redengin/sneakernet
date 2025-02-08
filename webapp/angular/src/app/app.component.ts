import { Component } from '@angular/core';

import { Toolbar } from './toolbar';

@Component({
  selector: 'app-root',
  imports: [ Toolbar ],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent {
}

