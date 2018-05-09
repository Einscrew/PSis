


int mutexLock(int region){
	return pthread_mutex_lock(&waitlock[region]);
}

int mutexUnlock(int region){
	return pthread_mutex_unlock(&waitlock[region]);
}