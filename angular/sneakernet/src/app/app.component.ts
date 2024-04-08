import {Component} from '@angular/core';
import {RouterOutlet} from '@angular/router';

import {MatToolbarModule} from '@angular/material/toolbar';
import {MatIconModule} from '@angular/material/icon';
import {MatListModule} from '@angular/material/list'; 
import {NgFor} from '@angular/common'; 
import {HttpClient, HttpClientModule} from '@angular/common/http';
import {Observable, lastValueFrom } from 'rxjs';
import {CommonModule} from '@angular/common';

@Component({
  selector: 'app-root',
  standalone: true,
  imports: [
      CommonModule,
      RouterOutlet,
      MatToolbarModule,
      MatIconModule,
      MatListModule,
      HttpClientModule,
      NgFor,
  ],
  templateUrl: './app.component.html',
  styleUrl: './app.component.css'
})
export class AppComponent {
  title = 'SneakerNet';

  constructor(private http: HttpClient) {}

  getCatalog() : Observable<SneakerNetFile[]> {
    return this.http.get<SneakerNetFile[]>('/catalog', {responseType:'json'});
  }

  delete(filename: string)
  {
    this.http.delete('/catalog/' + filename);
  }
}
export interface SneakerNetFile {
  filename: string,
  timestamp: string,
  size: number,
}
