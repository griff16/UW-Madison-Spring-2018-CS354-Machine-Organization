int arr[3000][500];
int main(){
    int i,j;
    for(i= 0; i<500;i++){
        for(j=0;j<3000;j++){
            arr[j][i] = i+j;
        }
    }

    return 0;
}
