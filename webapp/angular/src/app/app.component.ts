import { Component } from '@angular/core';


import {MatIconModule} from '@angular/material/icon';
import {MatButtonModule} from '@angular/material/button';
import {MatToolbarModule} from '@angular/material/toolbar';
@Component({
  selector: 'app-toolbar',
  templateUrl: './toolbar.html',
  imports: [MatToolbarModule, MatButtonModule, MatIconModule],
})
export class AppToolbar{
  title = 'Sneakernet';
};



import { RouterOutlet } from '@angular/router';
@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrl: './app.component.css',
  imports: [
    RouterOutlet,
    AppToolbar,
  ],
})
export class AppComponent {
}
